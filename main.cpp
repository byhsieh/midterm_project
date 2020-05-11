#include "accelerometer_handler.h"
#include "config.h"
#include "magic_wand_model_data.h"
#include "mbed.h"
#include "DA7212.h"

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "uLCD_4DGL.h"


DA7212 audio;
Serial pc(USBTX, USBRX);
InterruptIn button(SW2);
DigitalIn confirmbutton(SW3);
DigitalOut redled(LED1); 
DigitalOut greenled(LED2);

uLCD_4DGL uLCD(D1, D0, D2);

Thread t1;
Thread t2;
Thread t3;
Thread t4;

Timer timers;
Timer timer2;
Timer timer3;



int PredictGesture(float* output);
EventQueue queue1(32 * EVENTS_EVENT_SIZE);
EventQueue queue2(32 * EVENTS_EVENT_SIZE);
EventQueue queue3(32 * EVENTS_EVENT_SIZE);
EventQueue queue4(32 * EVENTS_EVENT_SIZE);
int gesture_result();
constexpr int kTensorArenaSize = 60 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

int interruptcall=0;
int player;
int endplaying=0;
int nowplaying=0;
int nextsong;
int numberofsongs=5;
int ready_to_load=5;
int song_select_mode=0;
int load_or_unload_mode=0;
int unload_mode=0;
int idC;
int irq=0;
int resetmusicplay=0;
int f=1;
int taikochoose=0;

int16_t waveform[kAudioTxBufferSize];
void playmusic(int);
void loadsong();
void load_or_unload();
void taikodisplay(int);
int gettaikolength(int );
// Set up logging.
static tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = &micro_error_reporter;

// Map the model into a usable data structure. This doesn't involve any
// copying or parsing, it's a very lightweight operation.
const tflite::Model* model = tflite::GetModel(g_magic_wand_model_data);
 TfLiteTensor* model_input;
int input_length;
tflite::MicroInterpreter* interpreter;
void playNote(int freq)
{ 
  if(f==1){
    for (int i = 0; i < kAudioTxBufferSize; i++)
    {
      waveform[i] = (int16_t) (sin((double)i * 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));
    }
    // the loop below will play the note for the duration of 1s

    for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize-15; ++j)
    {
        audio.spk.play(waveform, kAudioTxBufferSize);
    }
  }
  else{
    for (int i = 0; i < kAudioTxBufferSize; i++)
    {
      waveform[i] = (int16_t) ((0* 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));
    }
    audio.spk.play(waveform, kAudioTxBufferSize);
  }
} 
//void stop(){queue3.cancel(idC);}

class songs{
  public:
    void loadinfo(int*);
    string name;
    int length;
    void printinfo();
    void printname();
    int getinfo(int i){return info[i];}
    int getnotelength(int i){return notelength[i];}
    void unload();
    int speed;
  private:
    int* info;
    int* notelength;
};

void songs::loadinfo(int* x){
  info =new int[length];
  notelength = new int[length];
  for(int i=0;i<length;i++){
    info[i]=x[i];
  }
  int j=0;
  for(int i=length;i<2*length;i++){
    notelength[j]=x[i];
    j++;
  }
}
void songs::printinfo(){
  /*for(int i=0;i<length;i++){
    uLCD.printf("%d ",info[i]);
  }*/
  for(int i=0;i<length;i++){
    uLCD.printf("%d ",notelength[i]);
  }
}
void songs::printname(){
  int j=0;
  while(name[j]!='\0'){
    uLCD.printf("%c",name[j]);
    j++;
  }
}
void songs::unload(){
  name="0";
  length=0;
  delete []info;
  delete []notelength;
  //info=NULL;
}

songs songlist[5];

void song_list(){
  uLCD.cls();
  uLCD.textbackground_color(BLACK);
  uLCD.color(GREEN);
  song_select_mode=1;
  uLCD.printf("*** SONG LIST ***\n");
  for(int i=0;i<numberofsongs;i++){
    if(i==nowplaying)
      uLCD.textbackground_color(BLUE);
    else
      uLCD.textbackground_color(BLACK);
    uLCD.printf("#%d:",i);
    songlist[i].printname();
    uLCD.printf("\n");
  }
  uLCD.textbackground_color(BLACK);
  uLCD.printf("\n");
  uLCD.color(BLUE);
  uLCD.printf("*** SONG SEL ***\n");
  uLCD.printf("#0 forward :ring\n");
  uLCD.printf("#1 backward :slope\n");
  // uLCD.printf("load or unload :smile\n");
  int song_sel=gesture_result();
  /*if(result==2)
    load_or_unload();*/
  if(song_sel>=0){
    nextsong=song_sel;
    player=queue2.call_in(500,playmusic,1);
  }
  
  else
    uLCD.printf("errors");
  
  
  song_select_mode=0;

}
void load_or_unload(){
  uLCD.cls();
  load_or_unload_mode=1;
  uLCD.printf("load: ring\n");
  uLCD.printf("unload: slope\n");
  int result=gesture_result();
  load_or_unload_mode=0;
  if(result==0)
    loadsong();
  else if(result==1){
    uLCD.cls();
    uLCD.textbackground_color(BLACK);
    for(int i=0;i<numberofsongs;i++){
      if(i==0)
        uLCD.textbackground_color(BLUE);
      else
        uLCD.textbackground_color(BLACK);
      uLCD.printf("%d:",i);
      songlist[i].printname();
      uLCD.printf("\n");
    }
    uLCD.printf("which song to unload\n");
    uLCD.printf("forward :ring\n");
    uLCD.printf("backward :slope\n");
    unload_mode=1;
    int unloadnumber=gesture_result();
    unload_mode=0;
    songlist[unloadnumber].unload();
    int i=unloadnumber;
    while(i<numberofsongs-1){
      songlist[i]=songlist[i+1];
      //songlist[i].length=songlist[i+1].length;
      //songlist[i].name=songlist[i+1].name;
      i++;
    }
    numberofsongs--;
    ready_to_load--;
  }

}

void loadsong(){
  
  int i=0; 
  char a[2];
  char sp;
  int count=0;
  i=0;
  while(1){
    if(pc.getc()=='#')
      break;
  }

  while(true){
    if(pc.readable()){
      a[i]=pc.getc();
      count++;
      if(count==2){
        //a[count]='\0';
        sp=pc.getc();
        break;
      }
      i++;
    }
  }
  
  songlist[ready_to_load].speed=(int)sp -48;
  /*for(int j=0;j<2;j++){
    uLCD.printf("%c",a[j]);
  }*/
  int signalLength=(int)atof(a);
  
  songlist[ready_to_load].length=signalLength;
  
  i=0;
  char read[20];
  while(true){
    if(pc.readable()){
      read[i]=pc.getc();

      if(read[i]=='$'){
        read[i]='\0';
        break;
      }
      i++;
    }
  }
  songlist[ready_to_load].name=string(read);
  int signal[2*songlist[ready_to_load].length];
  int serialCount = 0;
  char serialInBuffer[32];
  i=0;  
  while(i < signalLength){
    if(pc.readable()){
      serialInBuffer[serialCount] = pc.getc();
      serialCount++;
      if(serialCount == 3 ){
        serialInBuffer[serialCount] = '\0';
        signal[i] = (int) atof(serialInBuffer);
        serialCount = 0;
        i++;
      }
    }
  }
  i=0;
  int j=signalLength;
  serialCount = 0;
  char in_noteL[100];
  while(i < signalLength){
    if(pc.readable()){
      in_noteL[i] = pc.getc();
      signal[j]= (int) in_noteL[i]-48;
      i++;
      j++;
    }
  }
  
  uLCD.cls();
  uLCD.printf("load done\n");
  int k=0;
  while(songlist[ready_to_load].name[k]!='\0'){
    uLCD.printf("%c",songlist[ready_to_load].name[k]);
    k++;
  }
  songlist[ready_to_load].loadinfo(signal);
  uLCD.printf("%d",songlist[ready_to_load].speed);
  ready_to_load++;
  numberofsongs++;
}

void mode_selection(){
  if(timers.read_ms()>1000){
    //queue3.cancel(idC);
    queue2.cancel(player);
    taikochoose=0;
    f=0;
    resetmusicplay=1;
    playNote(0);
    greenled=1;
    redled=0;
    wait_us(500);
    uLCD.cls();
    uLCD.color(GREEN);
    uLCD.printf("**** MODE SEL ****\n");
    uLCD.printf("#0 forward :ring\n");
    uLCD.printf("#1 backward :slope\n");
    uLCD.printf("#2 song_sel :smile\n");
    int mode_sel=gesture_result();
    if(mode_sel==0){
      if(nowplaying!=numberofsongs-1)
        nextsong=nowplaying+1;
      else
        nextsong=nowplaying;
      
      player=queue2.call_in(500,playmusic,1);
    }
    else if(mode_sel==1){
      if(nowplaying!=0)
        nextsong=nowplaying-1;
      else
        nextsong=nowplaying; 
      
      player=queue2.call_in(500,playmusic,1);
     
    }
    else if(mode_sel==2){
      song_list();

    }
    else{
      uLCD.printf("error\n");
    }
    //uLCD.printf("a: %d",a);
    timers.reset();
  }

  f=1;
}


void playmusic(int reset){
  
  f=1;
  redled=0;
  greenled=1;
  endplaying=0;
  nowplaying=nextsong;
  
  uLCD.cls();
  uLCD.circle(64, 50, 40, WHITE);
  uLCD.triangle(45, 25, 45, 75, 95, 50, WHITE);
  uLCD.color(GREEN);
  uLCD.locate(0, 13);
  uLCD.printf("Start Playing : #%d \n",nowplaying);
  uLCD.printf("** ");
  songlist[nowplaying].printname();
  uLCD.printf(" **");
  
  for(int i = 0; i < songlist[nowplaying].length; i++){
    int lengths = songlist[nowplaying].getnotelength(i);
    while(lengths--)
    { 
      if(resetmusicplay&& reset==0){
        break;
      }
      else if(reset==1){
        reset=0;
        resetmusicplay=0;
      }
  
      //int id=queue3.call_every(500,playNote, songlist[nowplaying].getinfo(i));
      playNote(songlist[nowplaying].getinfo(i));

      if(lengths ==0) {        
          wait(0.4/songlist[nowplaying].speed);
          
          playNote(0);
          wait(0.1/songlist[nowplaying].speed);
      }
      else{
        wait(0.5/songlist[nowplaying].speed);
      }
        

    }
  }
  

// Return the result of the last prediction
int PredictGesture(float* output) {
  // How many times the most recent gesture has been matched in a row
  static int continuous_count = 0;
  // The result of the last prediction
  static int last_predict = -1;

  // Find whichever output has a probability > 0.8 (they sum to 1)
  int this_predict = -1;
  for (int i = 0; i < label_num; i++) {
    if (output[i] > 0.8) this_predict = i;
  }

  // No gesture was detected above the threshold
  if (this_predict == -1) {
    continuous_count = 0;
    last_predict = label_num;
    return label_num;
  }

  if (last_predict == this_predict) {
    continuous_count += 1;
  } else {
    continuous_count = 0;
  }
  last_predict = this_predict;

  // If we haven't yet had enough consecutive matches for this gesture,
  // report a negative result
  if (continuous_count < config.consecutiveInferenceThresholds[this_predict]) {
    return label_num;
  }
  // Otherwise, we've seen a positive result, so clear all our variables
  // and report it
  continuous_count = 0;
  last_predict = -1;

  return this_predict;
}

int main(int argc, char* argv[]) {

  uLCD.cls();
  uLCD.color(GREEN);
  uLCD.locate(0,5);
  uLCD.printf("** User Manual **\n\n");
  uLCD.printf(" Press SW2 to\n enter MODE SEL\n and following\n the instructions.\n");

  t1.start(callback(&queue1, &EventQueue::dispatch_forever));
  t2.start(callback(&queue2, &EventQueue::dispatch_forever));
  t3.start(callback(&queue3, &EventQueue::dispatch_forever));
  t4.start(callback(&queue4, &EventQueue::dispatch_forever));
  timers.start();
  button.rise(queue1.event(mode_selection));
  songlist[0].name="little star";
  songlist[1].name="little bee";
  songlist[2].name="jingle bells";
  songlist[3].name="two tigers";
  songlist[4].name="train run";

  int song1[84]={261, 261, 392, 392, 440, 440, 392,
                349, 349, 330, 330, 294, 294, 261,
                392, 392, 349, 349, 330, 330, 294,
                392, 392, 349, 349, 330, 330, 294,
                261, 261, 392, 392, 440, 440, 392,
                349, 349, 330, 330, 294, 294, 261,
                1, 1, 1, 1, 1, 1, 2,
                1, 1, 1, 1, 1, 1, 2,
                1, 1, 1, 1, 1, 1, 2,
                1, 1, 1, 1, 1, 1, 2,
                1, 1, 1, 1, 1, 1, 2,
                1, 1, 1, 1, 1, 1, 2};
  songlist[0].length=42;
  songlist[0].loadinfo(song1);
  songlist[0].speed=1;
  
  int song2[98]={
    392,330,330,349,
    294,294,261,294,330,349,392,392,392,392,330,330,349,
    294,294,261,330,392,392,330,294,294,294,294,294,330,349,330,
    330,330,330,330,349,392,392,330,330,349,294,294,261,330,392,392,261,
    1,1,2,1,1,2,1,1,1,1,1,1,2,
    1,1,2,1,1,2,1,1,1,1,4,
    1,1,1,1,1,1,2,1,1,1,1,1,1,2,
    1,1,2,1,1,2,1,1,1,1,4
  };
  songlist[1].length=49;
  songlist[1].loadinfo(song2);
  songlist[1].speed=1;

  int song3[95]={
    330,330,330,330,330,330,330,392,261,294,
    330,349,349,349,349,349,330,330,330,294,294,330,294,
    392,330,330,330,330,330,330,330,392,261,294,330,349,349,349,
    349,349,330,330,392,392,349,294,261,
    1,1,2,1,1,2,1,1,1,1,4,
    1,1,2,1,1,1,1 ,1,1,1,1,2,2,
    1,1,2,1,1,2,1,1,1,1,4,
    1,1,2,1,1,1,1 ,1,1,1,1,2,2
  };
  songlist[2].length=47;
  songlist[2].loadinfo(song3);
  songlist[2].speed=1;

  int song4[64]={
    261,294,330,261,261,294,330,261,330,349,392,330,349,392,392,440,392,349,330,261,392,440,392,349,330,261,261,196,261,261,196,261,
    2,2,2,2,2,2,2,2,
    2,2,4,2,2,4,
    1,1,1,1,2,2,1,1,1,1,2,2,
    2,2,4,2,2,4
  };
  songlist[3].length=32;
  songlist[3].loadinfo(song4);
  songlist[3].speed=2;

  int song5[76]={
    392,392,330,261,392,392,330,261,294,330,349,349,330,349,349,392,392,330,392,330,294,330,261,349,294,294,294,330,261,261,261,294,330,349,294,261,247,261,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,2,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,2
  };
  songlist[4].length=38;
  songlist[4].loadinfo(song5);
  songlist[4].speed=1;

  nextsong=0;
  // wait(1);
  
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return -1;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  static tflite::MicroOpResolver<6> micro_op_resolver;
  micro_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
      tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,
                               tflite::ops::micro::Register_MAX_POOL_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                               tflite::ops::micro::Register_CONV_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,
                               tflite::ops::micro::Register_FULLY_CONNECTED());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                               tflite::ops::micro::Register_SOFTMAX());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,
                             tflite::ops::micro::Register_RESHAPE(), 1);
  // Build an interpreter to run the model with
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
 interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors
  interpreter->AllocateTensors();

  // Obtain pointer to the model's input tensor
  model_input = interpreter->input(0);
  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != config.seq_length) ||
      (model_input->dims->data[2] != kChannelNumber) ||
      (model_input->type != kTfLiteFloat32)) {
    error_reporter->Report("Bad input tensor parameters in model");
    return -1;
  }

 input_length = model_input->bytes / sizeof(float);

  TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
  if (setup_status != kTfLiteOk) {
    error_reporter->Report("Set up failed\n");
    return -1;
  }

  error_reporter->Report("Set up successful...\n");
  
  while(1){
    
    redled=1;
    greenled=0;
    wait(0.5);
    
  }
  
}
int gesture_result(){
  // Create an area of memory to use for input, output, and intermediate arrays.
  // The size of this will depend on the model you're using, and may need to be
  // determined by experimentation.
  redled=1;
  greenled=0;
  // Whether we should clear the buffer next time we fetch data
  bool should_clear_buffer = false;
  bool got_data = false;

  // The gesture index of the prediction
  int gesture_index;

  
  int getanswer=-1;
  int nowselect=nowplaying;
  int nowunload=0;
  while(true){
    redled=0;

    // Attempt to read new data from the accelerometer
    got_data = ReadAccelerometer(error_reporter, model_input->data.f,
                                 input_length, should_clear_buffer);
    
    // If there was no new data,
    // don't try to clear the buffer again and wait until next time
    if (!got_data) {
      should_clear_buffer = false;
      continue;
    }

    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      error_reporter->Report("Invoke failed on index: %d\n", begin_index);
      continue;
    }

    // Analyze the results to obtain a prediction
    gesture_index = PredictGesture(interpreter->output(0)->data.f);

    // Clear the buffer next time we read data
    should_clear_buffer = gesture_index < label_num;

    // Produce an output
    if (gesture_index < label_num) {
      error_reporter->Report(config.output_message[gesture_index]);
      getanswer=gesture_index;
      
      /***** songselect *****/
      if(song_select_mode /*&& !load_or_unload_mode&& !unload_mode*/){
        if(getanswer==0){
          if(nowselect==numberofsongs-1)
            nowselect=0;
          else
            nowselect++;
        }
          
        else if(getanswer==1){
          if(nowselect!=0)
            nowselect--;
          else{
            nowselect=numberofsongs-1;
          }
        }
        uLCD.cls();
        uLCD.locate(0,0);
        uLCD.color(GREEN);
        uLCD.printf("*** SONG LIST ***\n");
        for(int i=0;i<numberofsongs;i++){
          
          if(i==nowselect)
             uLCD.textbackground_color(BLUE);
          else{
            uLCD.textbackground_color(BLACK);
          }
          uLCD.printf("#%d: ",i);
          songlist[i].printname();
          uLCD.printf("\n");
        }
        uLCD.textbackground_color(BLACK);
        uLCD.printf("\n");
        uLCD.color(BLUE);
        uLCD.printf("*** SONG SEL ***\n");
        uLCD.printf("#0 forward :ring\n");
        uLCD.printf("#1 backward :slope\n");
        uLCD.color(RED);
        uLCD.printf("Press SW3 to check\n"); 
      }
      
      else{
        uLCD.textbackground_color(BLACK);
        uLCD.color(RED);
        uLCD.printf("your choice is %d ?\n",getanswer);
        uLCD.printf("Press SW3 to check\n"); 
        
      }
      
    }
    if(confirmbutton==0){
       uLCD.color(GREEN);
       uLCD.printf("Check Successful!\n");
       wait(0.3);
       break;
    }
    
  }
  
  if(song_select_mode/*&&!load_or_unload_mode*/){
    if(getanswer==2)
      return -2;
    else
      return nowselect;
  }
  else
    return getanswer;
}
