#include <user_config.h>
#include <SmingCore/SmingCore.h>

#include <Adafruit_NeoPixel/Adafruit_NeoPixel.h>
#include <math.h>

// Which pin on the Esp8266 is connected to the NeoPixels?
#define PIN1            15
#define PIN2            4
#define PIN3            5

// Which pin on the Esp8266 is connected to the NeoPixels?
#define PIN            4

// How many NeoPixels are attached to the Esp8266?
#define NUMPIXELS_1      250
#define NUMPIXELS_2      NUMPIXELS_1
#define NUMPIXELS_3      NUMPIXELS_1

// How many NeoPixels are attached to the Esp8266?
#define TESTAREAS      10

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(NUMPIXELS_1, PIN1, NEO_GRB );
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUMPIXELS_2, PIN2, NEO_GRB );
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(NUMPIXELS_3, PIN3, NEO_GRB );

void StartDemo(void);

Timer StripDemoTimer;
Timer ColorWipeTimer;
Timer TheaterChaseTimer;

int StripDemoType = 0;
int StripColor =0;
int StripNo=0;
int ChaseCycle=0;
int TheaterChaseQ=0;
int maxIntensity = 100;
bool inDemoMode = true;

int backgroundStart = 3;
int backgroundEnd = 247;

uint8 defaultRed = 255;
uint8 defaultGreen = 255;
uint8 defaultBlue = 170;
uint8 defaultIntensity = 35;

int UpdateDirection=0;
int UpdateState=0;

#define LED(intensity)	((uint8)((((float)intensity)*maxIntensity)/255.0))
#define LED_DEFAULT(intensity)	((uint8)((((float)intensity)*defaultIntensity)/255.0))

enum TestState
{
	TestState_Off = 0,
	TestState_On = 1,
	TestState_On2 = 2,
	TestState_Failed = 3,
	TestState_Ok = 4,
	TestState_On3 = 5,
	TestState_Unstable = 6,
};


class testarea {
  public:
	testarea();
	testarea(int start, int end);
	void setState(TestState state);
	TestState getState(void);
	int start;
	int end;
  private:
	TestState state;
};
testarea::testarea()
{
	this->start = 0;
	this->end = 0;
	this->state = TestState_Off;
}
testarea::testarea(int start, int end)
{
	this->start = start;
	this->end = end;
	this->state = TestState_Off;
}
void testarea::setState(TestState state)
{
	this->state = state;
}

TestState testarea::getState()
{
	return this->state ;
}

testarea testareas1[TESTAREAS];
testarea testareas2[TESTAREAS];
testarea testareas3[TESTAREAS];

void UpdateStrip(Adafruit_NeoPixel *strip, testarea *testareas)
{
	for (int i = 0; i < TESTAREAS; i++)
	{
		if ((testareas[i].start + testareas[i].end) != 0)
		{
			switch (testareas[i].getState())
			{
			case TestState_Failed:
			{
				uint8 ledvalue1 = LED(255);
				for (uint16_t j = testareas[i].start; j < testareas[i].end; j++)
				{
					strip->setPixelColor(j,ledvalue1,0,0);
				}
			}
			break;
			case TestState_Unstable:
			{
				uint8 ledvalue1 = LED(255);
				uint8 ledvalue2 = LED(90);
				for (uint16_t j = testareas[i].start; j < testareas[i].end; j++)
				{
					strip->setPixelColor(j,ledvalue1,ledvalue2,0);
				}
			}
			break;
			case TestState_Ok:
			{
				uint8 ledvalue1 = LED(255);
				for (uint16_t j = testareas[i].start; j < testareas[i].end; j++)
				{
					strip->setPixelColor(j,0,ledvalue1,0);
				}
			}
			break;
			case TestState_On:
			{
				float numLedsOn = ((float)(testareas[i].end - testareas[i].start)*UpdateState)/(20.0*2);
				uint8 ledvalue1 = LED(255);
				for (uint16_t j = testareas[i].start; j < testareas[i].end; j++)
				{
					if (j < ceil(testareas[i].start + numLedsOn) || j > floor(testareas[i].end - numLedsOn))
					{
						strip->setPixelColor(j,0,0,ledvalue1);
					} else {
						strip->setPixelColor(j,0,0,0);
					}
				}
			}
			break;
			case  TestState_On3:
			{
				float numLedsOn = ((float)(testareas[i].end - testareas[i].start)/5);
				float numLedsStart = (((float)(testareas[i].end - testareas[i].start)-numLedsOn)*UpdateState)/(20.0);
				uint8 ledvalue1 = LED(255);
				for (uint16_t j = testareas[i].start; j < testareas[i].end; j++)
				{
					float intensity = (float)ledvalue1;
					intensity = intensity - ((UpdateState*(intensity*0.9))/20.0);
					for (uint16_t j = testareas[i].start; j < testareas[i].end; j++)
					{
						strip->setPixelColor(j,0,0,(uint8)intensity);
					}
				}
			}
			break;
			case TestState_On2:
			{
				float numLedsOn = ((float)(testareas[i].end - testareas[i].start)*UpdateState)/(20.0*2);
				uint8 ledvalue1 = LED(255);
				for (uint16_t j = testareas[i].start; j < testareas[i].end; j++)
				{
					if (j < ceil(testareas[i].start + numLedsOn) || j > floor(testareas[i].end - numLedsOn))
					{
						strip->setPixelColor(j,0,0,ledvalue1);
					} else {
						strip->setPixelColor(j,0,0,0);
					}
				}
			}
			break;
			default:
			{
				uint8 ledvalueR = LED_DEFAULT(defaultRed);
				uint8 ledvalueG = LED_DEFAULT(defaultGreen);
				uint8 ledvalueB = LED_DEFAULT(defaultBlue);
				for (uint16_t j = testareas[i].start; j < testareas[i].end; j++)
				{
					strip->setPixelColor(j,ledvalueR,ledvalueG,ledvalueB);
				}
			}
			break;
			}
		}
	}
}

void UpdatePixels() {

	uint8 ledvalueR = LED_DEFAULT(defaultRed);
	uint8 ledvalueG = LED_DEFAULT(defaultGreen);
	uint8 ledvalueB = LED_DEFAULT(defaultBlue);
	for (uint16_t i = 0; i < strip1.numPixels(); i++)
	{
		if (i > backgroundStart && i < backgroundEnd)
		{
			if (i < strip1.numPixels())
				strip1.setPixelColor(i,ledvalueR,ledvalueG,ledvalueB);
			if (i < strip2.numPixels())
				strip2.setPixelColor(i,ledvalueR,ledvalueG,ledvalueB);
			if (i < strip3.numPixels())
				strip3.setPixelColor(i,ledvalueR,ledvalueG,ledvalueB);
		} else {
			if (i < strip1.numPixels())
				strip1.setPixelColor(i,0,0,0);
			if (i < strip2.numPixels())
				strip2.setPixelColor(i,0,0,0);
			if (i < strip3.numPixels())
				strip3.setPixelColor(i,0,0,0);
		}
	}
	UpdateStrip(&strip1, testareas1);
	UpdateStrip(&strip2, testareas2);
	wdt_feed();
	UpdateStrip(&strip3, testareas3);
	wdt_feed();
	strip1.show();
	wdt_feed();
	strip2.show();
	wdt_feed();
	strip3.show();

	if (UpdateState >= 20)
	{
		UpdateDirection=0;
	} else if (UpdateState <= 0)
	{
		UpdateDirection=1;
	}
	if (UpdateDirection)
	{
		UpdateState++;
	} else
	{
		UpdateState--;
	}
}
void StartDemo2() {
	if (!inDemoMode)
		return;
	Serial.print("NeoPixel Demo type: ");
	Serial.println(StripDemoType);
   	StripNo = 0;   //start from led index 0

   	UpdateDirection=0;
   	UpdateState=0;
   	switch(StripDemoType){           // select demo type
   	    case 0:
   	    	testareas1[0].setState(TestState_Unstable);
   	    	testareas1[1].setState(TestState_Ok);
   	    	testareas1[4].setState(TestState_On);
   	    	testareas1[2].setState(TestState_On2);
   	    	testareas1[3].setState(TestState_Ok);
   	    	testareas1[5].setState(TestState_Unstable);
   			testareas1[6].setState(TestState_Ok);
   			testareas1[7].setState(TestState_On);
   			testareas1[8].setState(TestState_On3);
   			testareas1[9].setState(TestState_Ok);
   	    	break;
   	    case 1:
   			testareas1[0].setState(TestState_Ok);
   			testareas1[1].setState(TestState_On);
   			testareas1[4].setState(TestState_Ok);
   			testareas1[2].setState(TestState_Ok);
   			testareas1[3].setState(TestState_Unstable);
   			testareas1[5].setState(TestState_Ok);
   			testareas1[6].setState(TestState_On);
   			testareas1[7].setState(TestState_Ok);
   			testareas1[8].setState(TestState_Ok);
   			testareas1[9].setState(TestState_Unstable);
   			break;
   	    case 2:
   			testareas1[0].setState(TestState_On);
   			testareas1[1].setState(TestState_Unstable);
   			testareas1[4].setState(TestState_On3);
   			testareas1[2].setState(TestState_Unstable);
   			testareas1[3].setState(TestState_On);
   			testareas1[5].setState(TestState_On);
   			testareas1[6].setState(TestState_Unstable);
   			testareas1[7].setState(TestState_On2);
   			testareas1[8].setState(TestState_Unstable);
   			testareas1[9].setState(TestState_On);
   			break;
   	    case 3:
   			testareas1[0].setState(TestState_Ok);
   			testareas1[1].setState(TestState_On);
   			testareas1[4].setState(TestState_Unstable);
   			testareas1[2].setState(TestState_On2);
   			testareas1[3].setState(TestState_Ok);
   			testareas1[5].setState(TestState_Ok);
   			testareas1[6].setState(TestState_On);
   			testareas1[7].setState(TestState_Unstable);
   			testareas1[8].setState(TestState_On2);
   			testareas1[9].setState(TestState_Ok);
   			break;
   	    default:
   			StripDemoType=0;
   			break;
   	}
   	switch(StripDemoType){           // select demo type
   	    case 0:
   	    	testareas2[0].setState(TestState_Unstable);
   	    	testareas2[1].setState(TestState_Ok);
   	    	testareas2[4].setState(TestState_On);
   	    	testareas2[2].setState(TestState_On2);
   	    	testareas2[3].setState(TestState_Ok);
   	    	testareas2[5].setState(TestState_Unstable);
   			testareas2[6].setState(TestState_Ok);
   			testareas2[7].setState(TestState_On);
   			testareas2[8].setState(TestState_On2);
   			testareas2[9].setState(TestState_Ok);
   	    	break;
   	    case 1:
   			testareas2[0].setState(TestState_Ok);
   			testareas2[1].setState(TestState_On3);
   			testareas2[4].setState(TestState_Ok);
   			testareas2[2].setState(TestState_Ok);
   			testareas2[3].setState(TestState_Unstable);
   			testareas2[5].setState(TestState_Ok);
   			testareas2[6].setState(TestState_On);
   			testareas2[7].setState(TestState_Ok);
   			testareas2[8].setState(TestState_Ok);
   			testareas2[9].setState(TestState_Unstable);
   			break;
   	    case 2:
   			testareas2[0].setState(TestState_On);
   			testareas2[1].setState(TestState_Unstable);
   			testareas2[4].setState(TestState_On2);
   			testareas2[2].setState(TestState_Unstable);
   			testareas2[3].setState(TestState_On);
   			testareas2[5].setState(TestState_On3);
   			testareas2[6].setState(TestState_Unstable);
   			testareas2[7].setState(TestState_On2);
   			testareas2[8].setState(TestState_Unstable);
   			testareas2[9].setState(TestState_On);
   			break;
   	    case 3:
   			testareas2[0].setState(TestState_Ok);
   			testareas2[1].setState(TestState_On);
   			testareas2[4].setState(TestState_Unstable);
   			testareas2[2].setState(TestState_On2);
   			testareas2[3].setState(TestState_Ok);
   			testareas2[5].setState(TestState_Ok);
   			testareas2[6].setState(TestState_On);
   			testareas2[7].setState(TestState_Unstable);
   			testareas2[8].setState(TestState_On2);
   			testareas2[9].setState(TestState_Ok);
   			break;
   	    default:
   			StripDemoType=0;
   			break;
   	}
	switch(StripDemoType){           // select demo type
		case 0:
			testareas3[0].setState(TestState_Unstable);
			testareas3[1].setState(TestState_Ok);
			testareas3[4].setState(TestState_On);
			testareas3[2].setState(TestState_On3);
			testareas3[3].setState(TestState_Ok);
			testareas3[5].setState(TestState_Unstable);
			testareas3[6].setState(TestState_Ok);
			testareas3[7].setState(TestState_On);
			testareas3[8].setState(TestState_On2);
			testareas3[9].setState(TestState_Ok);
			break;
		case 1:
			testareas3[0].setState(TestState_Ok);
			testareas3[1].setState(TestState_On3);
			testareas3[4].setState(TestState_Ok);
			testareas3[2].setState(TestState_Ok);
			testareas3[3].setState(TestState_Unstable);
			testareas3[5].setState(TestState_Ok);
			testareas3[6].setState(TestState_On3);
			testareas3[7].setState(TestState_Ok);
			testareas3[8].setState(TestState_Ok);
			testareas3[9].setState(TestState_Unstable);
			break;
		case 2:
			testareas3[0].setState(TestState_On);
			testareas3[1].setState(TestState_Failed);
			testareas3[4].setState(TestState_On2);
			testareas3[2].setState(TestState_Unstable);
			testareas3[3].setState(TestState_On);
			testareas3[5].setState(TestState_On);
			testareas3[6].setState(TestState_Unstable);
			testareas3[7].setState(TestState_On2);
			testareas3[8].setState(TestState_Unstable);
			testareas3[9].setState(TestState_On);
			break;
		case 3:
			testareas3[0].setState(TestState_Ok);
			testareas3[1].setState(TestState_On3);
			testareas3[4].setState(TestState_Unstable);
			testareas3[2].setState(TestState_On3);
			testareas3[3].setState(TestState_Ok);
			testareas3[5].setState(TestState_Ok);
			testareas3[6].setState(TestState_On);
			testareas3[7].setState(TestState_Unstable);
			testareas3[8].setState(TestState_On2);
			testareas3[9].setState(TestState_Ok);
			break;
		default:
			StripDemoType=0;
			break;
	}
	StripDemoType++;
	if (StripDemoType > 3)
	{
		StripDemoType = 0;
	}
}



void InitDemo2() {
	if (!inDemoMode)
	{
		StripDemoTimer.initializeMs(5000, StartDemo2).start();  //start demo
		ColorWipeTimer.initializeMs(45, UpdatePixels).start();  //start demo
		return;
	}

	Serial.print("NeoPixel init step: ");
	Serial.println(StripDemoType);
	static int StripInitType = 0;

	switch(StripInitType){           // select demo type
    case 0:
    	/* first test */
		for (uint16_t i = 0; i < strip1.numPixels(); i++)
		{
			strip1.setPixelColor(i,0,0,255);
			strip2.setPixelColor(i,0,255,0);
			strip3.setPixelColor(i,255,0,0);
		}
		break;
    case 1:
		for (uint16_t i = 0; i < strip1.numPixels(); i++)
		{
			strip2.setPixelColor(i,0,0,255);
			strip3.setPixelColor(i,0,255,0);
			strip1.setPixelColor(i,255,0,0);
		}
		break;
    case 2:
    	for (uint16_t i = 0; i < strip1.numPixels(); i++)
		{
			strip3.setPixelColor(i,0,0,255);
			strip1.setPixelColor(i,0,255,0);
			strip2.setPixelColor(i,255,0,0);
		}
    	break;
    case 3:
		for (uint16_t i = 0; i < strip1.numPixels(); i++)
		{
			strip1.setPixelColor(i,255,255,255);
			strip2.setPixelColor(i,255,255,255);
			strip3.setPixelColor(i,255,255,255);
		}
		break;
    case 4:
    	for (uint16_t i = 0; i < strip1.numPixels(); i++)
		{
			strip1.setPixelColor(i,255-(i%255),(i+128)%255,i%255);
			strip2.setPixelColor(i,255-(i%255),(i+128)%255,i%255);
			strip3.setPixelColor(i,255-(i%255),(i+128)%255,i%255);
		}
        break;
    default:
    	StripDemoTimer.initializeMs(5000, StartDemo2).start();  //start demo
    	ColorWipeTimer.initializeMs(45, UpdatePixels).start();  //start demo
		break;
	}
	wdt_feed();
	strip1.show();
	strip2.show();
	wdt_feed();
	strip3.show();
	StripInitType++;
}



// Will be called when WiFi station was connected to AP
void connect_Ok()
{
	Serial.print("I'm CONNECTED - ");
	Serial.println(WifiStation.getIP().toString());

	//You can put here other job like web,tcp etc.
}

// Will be called when WiFi station timeout was reached
void connect_Fail()
{
	Serial.println("I'm NOT CONNECTED!");
	WifiStation.waitConnection(connect_Ok, 10, connect_Fail); // Repeat and check again
}

void printSerialHelp()
{
	Serial.println("Available commands:");
	Serial.println("\tU:123");
	Serial.println("\t   Configure animation speed, default is 35 ms (range 35 to 999)");
	
	Serial.println("\tU:123");
	Serial.println("\t   Configure animation speed, default is 35 ms (range 35 to 999)");
	
	Serial.println("\tD:123:223:43:50:3:247");
	Serial.println("\t   Configure background color to red=123, green=223, blue=43 with");
	Serial.println("\t   max intensity scaled to 50, starting at LED 3 and ending at LED 247");
	
	Serial.println("\tC:1:2:100:200");
	Serial.println("\t   Configure row 1 (range 1 to 3) site 2 (range 0 to 9), led start 100 led end 200");
	
	Serial.println("\tS:1:2:4");
	Serial.println("\t   Set mode 4 (TestState_Ok) on row 1 (range 1 to 3) site 2 (range 0 to 9)");
	Serial.println("\t    Modes:");
	Serial.println("\t     TestState_Off = 0");
	Serial.println("\t     TestState_On = 1");
	Serial.println("\t     TestState_On2 = 2");
	Serial.println("\t     TestState_Unstable = 3");
	Serial.println("\t     TestState_Ok = 4");
	Serial.println("\t     TestState_On3 = 5");
	
	Serial.println("\tI:255");
	Serial.println("\t   Set Max intensity to value (0 to 255)");
}

bool parseSerialData(char *data, int len)
{
	if (len < 3)
		return false;
	int row = data[2] - '0';
	int site = data[4] - '0';
	int ledStart = 0;
	int ledEnd = 0;
	int mode = 0;
	int index = 0;
	int indexStart = 0;
	int div = 1000;
	int temp = 0;
	switch (data[0])
	{
	case 'D':
		Serial.println("Default color Command");
		index = 2;
		indexStart = index;
		temp = 0;
		while(data[index] != ':' && data[index] != '\n' && data[index] >= '0' && data[index] <= '9' && div > 5)
		{
			div = div /10;
			temp += div*(data[index] - '0');
			index++;
		}
		temp = temp / div;
		if (temp < 0 || temp > 255)
		{
			Serial.print(temp);
			Serial.println(" Red color out of range 0 < color <= 255");
			return false;
		}
		defaultRed = (uint8)temp;
		index++;
		div = 1000;
		indexStart = index;
		temp = 0;
		while(data[index] != ':' && data[index] != '\n' && data[index] >= '0' && data[index] <= '9' && div > 5)
		{
			div = div /10;
			temp += div*(data[index] - '0');
			index++;
		}
		temp = temp / div;
		if (temp < 0 || temp > 255)
		{
			Serial.print(temp);
			Serial.println(" Blue color out of range 0 < color <= 255");
			return false;
		}
		defaultBlue = (uint8)temp;
		index++;
		div = 1000;
		indexStart = index;
		temp = 0;
		while(data[index] != ':' && data[index] != '\n' && data[index] >= '0' && data[index] <= '9' && div > 5)
		{
			div = div /10;
			temp += div*(data[index] - '0');
			index++;
		}
		temp = temp / div;
		if (temp < 0 || temp > 255)
		{
			Serial.print(temp);
			Serial.println(" Green color out of range 0 < color <= 255");
			return false;
		}
		defaultGreen = (uint8)temp;
		index++;
		div = 1000;
		indexStart = index;
		temp = 0;
		while(data[index] != ':' && data[index] != '\n' && data[index] >= '0' && data[index] <= '9' && div > 5)
		{
			div = div /10;
			temp += div*(data[index] - '0');
			index++;
		}
		temp = temp / div;
		if (temp < 0 || temp > 255)
		{
			Serial.print(temp);
			Serial.println(" Intensity out of range 0 < intensity <= 255");
			return false;
		}
		defaultIntensity = (uint8)temp;
		index++;
		div = 1000;
		indexStart = index;
		temp = 0;
		while(data[index] != ':' && data[index] != '\n' && data[index] >= '0' && data[index] <= '9' && div > 5)
		{
			div = div /10;
			temp += div*(data[index] - '0');
			index++;
		}
		temp = temp / div;
		if (temp < 0 || temp >= NUMPIXELS_1)
		{
			Serial.print(temp);
			Serial.print(" Start position out of range 0 < pos <= ");
			Serial.println(NUMPIXELS_1);
			return false;
		}
		backgroundStart = (uint8)temp;		index++;
		div = 1000;
		indexStart = index;
		temp = 0;
		while(data[index] != ':' && data[index] != '\n' && data[index] >= '0' && data[index] <= '9' && div > 5)
		{
			div = div /10;
			temp += div*(data[index] - '0');
			index++;
		}
		temp = temp / div;
		if (temp < 0 || temp > NUMPIXELS_1)
		{
			Serial.print(temp);
			Serial.print(" End position out of range 0 < pos <= ");
			Serial.println(NUMPIXELS_1);
			return false;
		}
		backgroundEnd = (uint8)temp;		
		Serial.print("Start LED: ");
		Serial.print(backgroundStart);
		Serial.print("End LED: ");
		Serial.print(backgroundEnd);
		Serial.print("Red: ");
		Serial.print(defaultRed);
		Serial.print(" Green: ");
		Serial.print(defaultGreen);
		Serial.print(" Blue: ");
		Serial.print(defaultBlue);
		Serial.print(" Intensity: ");
		Serial.println(defaultIntensity);
		return true;
		break;
	case 'I':
		Serial.println("Set Max Intensity Command");
		index = 2;
		indexStart = index;
		while(data[index] != ':' && data[index] != '\n'  && data[index] >= '0' && data[index] <= '9' && div > 5)
		{
			div = div /10;
			temp += div*(data[index] - '0');
			index++;
		}
		temp = temp / div;
		if (temp < 0 || temp > 255)
		{
			Serial.print(temp);
			Serial.println(" Intensity out of range 0 < intensity <= 255");
			return false;
		}
		Serial.print("Max Intensity: ");
		Serial.println(temp);
		maxIntensity = temp;
		return true;
		break;
	case 'U':
		Serial.println("Set Animation Speed Command");
		index = 2;
		indexStart = index;
		while(data[index] != ':' && data[index] != '\n'  && data[index] >= '0' && data[index] <= '9' && div > 5)
		{
			div = div /10;
			temp += div*(data[index] - '0');
			index++;
		}
		temp = temp / div;
		if (temp < 30 || temp > 999)
		{
			Serial.print(temp);
			Serial.println(" Intensity out of range 30 < delay <= 999");
			return false;
		}
		Serial.print("Speed is set to: ");
		Serial.print(temp);
		Serial.println("ms");
		ColorWipeTimer.initializeMs(temp, UpdatePixels).start();
		return true;
		break;
	case 'C':
		Serial.println("Configure Command");
		if (row < 1 || row > 3)
		{
			Serial.print(row);
			Serial.println(" Row position out of range 1 =< pos =< 3");
			return false;
		}
		if (site < 1 || site > site)
		{
			Serial.print(site);
			Serial.print(" Site position out of range 1 =< pos =< ");
			Serial.println(site);
			return false;
		}
		index = 6;
		indexStart = index;
		while(data[index] != ':' && data[index] >= '0' && data[index] <= '9' && div > 5)
		{
			div = div /10;
			ledStart += div*(data[index] - '0');
			index++;
		}
		ledStart = ledStart / div;
		if (ledStart < 0 || ledStart >= NUMPIXELS_1)
		{
			Serial.print(ledStart);
			Serial.print(" Start position out of range 0 > pos >= ");
			Serial.println(NUMPIXELS_1);
			return false;
		}
		index++;
		indexStart = index;
		div = 1000;
		while(data[index] != ':' && data[index] != '\n'  && data[index] >= '0' && data[index] <= '9' && div > 5)
		{
			div = div /10;
			ledEnd += div*(data[index] - '0');
			index++;
		}
		ledEnd = ledEnd / div;

		if (ledEnd < 0 || ledEnd > NUMPIXELS_1 || ledEnd < ledStart)
		{
			Serial.print(ledEnd);
			Serial.print(" End position out of range 0 > pos >= ");
			Serial.println(NUMPIXELS_1);
			Serial.print("End position must be larger than start position ");
			Serial.println(ledStart);
			return false;
		}
		Serial.print("Row: ");
		Serial.print(row);
		Serial.print(" Site: ");
		Serial.print(site);
		Serial.print(" Start: ");
		Serial.print(ledStart);
		Serial.print(" End: ");
		Serial.println(ledEnd);
		switch (row)
		{
		case 1:
			testareas1[site].start = ledStart;
			testareas1[site].end = ledEnd;
			break;
		case 2:
			testareas2[site].start = ledStart;
			testareas2[site].end = ledEnd;
			break;
		case 3:
			testareas3[site].start = ledStart;
			testareas3[site].end = ledEnd;
			break;
		}
		return true;
		break;
	case 'S':
		Serial.println("Set Command");
		if (row < 1 || row > 3)
		{
			Serial.print(row);
			Serial.println(" Row position out of range 1 =< pos =< 3");
			return false;
		}
		if (site < 1 || site > site)
		{
			Serial.print(site);
			Serial.print(" Site position out of range 1 =< pos =< ");
			Serial.println(site);
			return false;
		}
		mode =  data[6] - '0';
		if (mode < 1 || mode > 5)
		{
			Serial.print(row);
			Serial.println(" Mode out of range 1 =< pos =< 5");
			return false;
		}
		switch (row)
		{
		case 1:
			testareas1[site].setState((TestState)mode);
			break;
		case 2:
			testareas2[site].setState((TestState)mode);
			break;
		case 3:
			testareas3[site].setState((TestState)mode);
			break;
		}
		Serial.print("Row: ");
		Serial.print(row);
		Serial.print(" Site: ");
		Serial.print(site);
		Serial.print(" Mode: ");
		switch (testareas3[site].getState())
		{
		case TestState_Unstable:
			Serial.println("Failed");
			break;
		case TestState_Off:
			Serial.println("Off");
			break;
		case TestState_Ok:
			Serial.println("Ok");
			break;
		case TestState_On2:
			Serial.println("On 2");
			break;
		case TestState_On:
			Serial.println("On");
			break;
		case TestState_On3:
			Serial.println("On 3");
			break;
		}

		return true;
		break;
	}

	return false;
}

unsigned charReceived = 0;
unsigned numCallback = 0;
bool useRxFlag = true;
#define BUF_LEN	30
char receivedText[BUF_LEN];

void onData(Stream& stream, char arrivedChar, unsigned short availableCharsCount)
{
	while (stream.available())
	{
		receivedText[charReceived] = stream.read();
		charReceived++;
		if (receivedText[charReceived-1] == '\n') // Lets show data!
		{
			if (parseSerialData(receivedText, charReceived -1))
			{
				if (inDemoMode)
				{
					inDemoMode = false;
					for (int i = 0; i < TESTAREAS; i++)
					{
						testareas1[i].start = 0;
						testareas2[i].start = 0;
						testareas3[i].start = 0;
						testareas1[i].end = 0;
						testareas2[i].end = 0;
						testareas3[i].end = 0;
						testareas1[i].setState(TestState_Off);
						testareas2[i].setState(TestState_Off);
						testareas3[i].setState(TestState_Off);
					}
					parseSerialData(receivedText, charReceived -1);
				}
			} else {
				printSerialHelp();
			}
			
			Serial.println("<New line received>");
			for (int i = 0; i < (charReceived -1); i++)
			{
				Serial.print(receivedText[i]);
			}
			charReceived = 0;
			Serial.println();
		}
		if (charReceived >=  BUF_LEN)
		{
			charReceived = 0;
		}
	}
}


void init()
{
	//pinMode(5, OUTPUT);
	//	digitalWrite(5,1);
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(false); // Disable debug output to serial

	Serial.print("NeoPixel demo .. start");


    // Wifi could be used eg. for switching Neopixel from internet
	// could be also dissabled if no needed

	//WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(false);
	WifiAccessPoint.enable(false);
	//WifiStation.waitConnection(connect_Ok, 20, connect_Fail);



	StripDemoType =0;  //demo index to be displayed

	strip1.begin();  //init port
	strip2.begin();  //init port
	strip3.begin();  //init port

	for (int i = 0; i < TESTAREAS; i++)
	{
		testareas1[i].start = 0;
		testareas1[i].end = 0;
		testareas1[i].setState(TestState_Off);
		testareas2[i].start = 0;
		testareas2[i].end = 0;
		testareas2[i].setState(TestState_Off);
		testareas3[i].start = 0;
		testareas3[i].end = 0;
		testareas3[i].setState(TestState_Off);
	}
	testareas1[0].start = 20;
	testareas1[0].end = 65;
	testareas1[1].start = 65;
	testareas1[1].end = 109;
	testareas1[2].start = 140;
	testareas1[2].end = 185;
	testareas1[3].start = 200;
	testareas1[3].end = 240;
	testareas1[4].start = 110;
	testareas1[4].end = 134;
	testareas1[5].start = 300;
	testareas1[5].end = 365;
	testareas1[6].start = 400;
	testareas1[6].end = 550;
	testareas1[7].start = 560;
	testareas1[7].end = 600;
	testareas1[8].start = 620;
	testareas1[8].end = 670;
	testareas1[9].start = 700;
	testareas1[9].end = 730;

	testareas2[0].start = 20;
	testareas2[0].end = 105;
	testareas2[1].start = 165;
	testareas2[1].end = 200;
	testareas2[2].start = 210;
	testareas2[2].end = 250;

	testareas3[0].start = 20;
	testareas3[0].end = 165;
	testareas3[1].start = 165;
	testareas3[1].end = 200;
	testareas3[2].start = 200;
	testareas3[2].end = 245;

	StripDemoTimer.initializeMs(4000, InitDemo2).start();  //start demo

	Serial.setCallback(onData);
}



