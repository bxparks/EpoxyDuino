#line 2 "SPIMockTest"

#include <Arduino.h>
#include <AUnit.h>
#include <SPI.h>

using aunit::TestRunner;

#define csPin 10

/* Macros to make starting and ending easier*/
#define SPI_START digitalWrite(csPin, LOW)
#define SPI_END digitalWrite(csPin, HIGH)

/*Test writing 8 bits with SPI*/
test(SPIMockTestBase, write8)
{

	SPI_START;

	/*Write 8 bits*/
	SPI.transfer(0x8F);

	SPI_END;

	/*Get value that we wrote*/
	uint16_t writtenValue = SPI.transferWriteValue(1);
	SPI.resetWriteBuffer();

	/*Make sure we actually wrote 0x8F*/
	assertEqual(0x8F, writtenValue);

}

/*Test writing 16 bits with SPI*/
test(SPIMockTestBase, write16)
{

	SPI_START;

	/*Write 16 bits*/
	SPI.transfer16(0x5F5F);

	SPI_END;

	/*Get value that we wrote*/
	uint16_t writtenValue = SPI.transferWriteValue(1);
	SPI.resetWriteBuffer();

	/*Make sure we actually wrote 0x8F*/
	assertEqual(0x5F5F, writtenValue);


}

/*Test reading a given 8 bits with SPI*/
test(SPIMockTestBase, read8)
{

	/*Setting expected read value*/
	SPI.transferReturnValue(0x5F);

	SPI_START;

	/*Write 8 bits so we can get a read 8 bits*/
	uint16_t readValue = SPI.transfer(0x8F);

	SPI_END;
	SPI.resetWriteBuffer();

	/*Make sure we actually read 0x8F*/
	assertEqual(0x5F, readValue);

}

/*Test reading a given 16 bits with SPI*/
test(SPIMockTestBase, read16)
{

	/*Setting expected read value*/
	SPI.transferReturnValue(0x5F5F);

	SPI_START;

	/*Write 8 bits so we can get a read 8 bits*/
	uint16_t readValue = SPI.transfer16(0x8F8F);

	SPI_END;

	SPI.resetWriteBuffer();

	/*Make sure we actually read 0x8F*/
	assertEqual(0x5F5F, readValue);

}

/*Test that we can both read and write values at the same time
  8 bits at a time*/
test(SPIMockTestBase, readWrite8)
{

	/*Setting expected read value*/
	SPI.transferReturnValue(0xFF);

	SPI_START;

	/*Write 8 bits*/
	uint16_t readValue = SPI.transfer(0x88);

	SPI_END;

	/*get value we wrote*/
	uint16_t writtenValue = SPI.transferWriteValue(1);

	SPI.resetWriteBuffer();

	/*Ensure we wrote read and wrote correct 8 bits*/
	assertEqual(0xFF, readValue);
	assertEqual(0x88, writtenValue);

}
/*Test that we can both read and write values at the same time
  16 bits at a time*/
test(SPIMockTestBase, readWrite16)
{

	/*Setting expected read value*/
	SPI.transferReturnValue(0xFF88);

	SPI_START;

	/*Write 8 bits*/
	uint16_t readValue = SPI.transfer16(0x88FF);

	SPI_END;

	/*get value we wrote*/
	uint16_t writtenValue = SPI.transferWriteValue(1);

	SPI.resetWriteBuffer();

	/*Ensure we wrote read and wrote correct 8 bits*/
	assertEqual(0xFF88, readValue);
	assertEqual(0x88FF, writtenValue);

}

/*Test that we are able to see the return value of the last two SPI Writes*/
test(SPIMockTestMultiple, dualWrite){

	SPI_START;

	/*two SPI transfers*/
	SPI.transfer(0x11);
	SPI.transfer(0x22);

	SPI_END;

	/* get the two previous writes*/
	uint16_t firstCall = SPI.transferWriteValue(1);
	uint16_t secondCall = SPI.transferWriteValue(2);

	SPI.resetWriteBuffer();

	/* Make sure they are equal*/
	assertEqual(0x11, firstCall);
	assertEqual(0x22, secondCall);


}
/*Test that we are able to see the return value of the last three SPI Writes*/

test(SPIMockTestMultiple, tripleWrite){
	
	SPI_START;

	/*three SPI transfers*/
	SPI.transfer(0x11);
	SPI.transfer(0x22);
	SPI.transfer(0x33);

	SPI_END;

	/* get the three previous writes*/
	uint16_t firstCall = SPI.transferWriteValue(1);
	uint16_t secondCall = SPI.transferWriteValue(2);
	uint16_t thirdCall = SPI.transferWriteValue(3);

	SPI.resetWriteBuffer();

	/* Make sure they are equal*/
	assertEqual(0x11, firstCall);
	assertEqual(0x22, secondCall);
	assertEqual(0x33, thirdCall);


}
/*Test that we are able to see the return value of the last four SPI Writes*/

test(SPIMockTestMultiple, quadWrite){
	
	SPI_START;

	/*three SPI transfers*/
	SPI.transfer(0x11); //buffer[0]
	SPI.transfer(0x22); //buffer[1]
	SPI.transfer(0x33); //buffer[2]
	SPI.transfer(0x44); //buffer[3]

	SPI_END;

	/* get the four previous writes*/
	uint16_t firstCall = SPI.transferWriteValue(1); //buffer[(4-1)%4] = buffer[3]
	uint16_t secondCall = SPI.transferWriteValue(2); //buffer[(3-1)%4] = buffer[2]
	uint16_t thirdCall = SPI.transferWriteValue(3); //buffer[(2-1)%4] = buffer[1]
	uint16_t fourthCall = SPI.transferWriteValue(4); //buffer[(1-1)%4] = buffer[0]

	SPI.resetWriteBuffer();

	/* Make sure they are equal*/
	assertEqual(0x11, firstCall);
	assertEqual(0x22, secondCall);
	assertEqual(0x33, thirdCall);
	assertEqual(0x44, fourthCall);


}
/*Test that we are able to see the return value of the last 4 SPI Writes
  where we have written five times so we should have a rollover*/

test(SPIMockTestMultiple, pentaWrite){
		
	SPI_START;

	/*three SPI transfers*/
	SPI.transfer(0x11); //buffer[0]
	SPI.transfer(0x22); //buffer[1]
	SPI.transfer(0x33); //buffer[2]
	SPI.transfer(0x44); //buffer[3]
	SPI.transfer(0x55); //buffer[0]

	SPI_END;

	/* get the four previous writes*/
	uint16_t secondCall = SPI.transferWriteValue(2); //buffer[(1-1)%4] = buffer[0]
	uint16_t thirdCall = SPI.transferWriteValue(3);
	uint16_t fourthCall = SPI.transferWriteValue(4);
	uint16_t fifthCall = SPI.transferWriteValue(5);

	uint8_t callCount = SPI.getCallCount();

	SPI.resetWriteBuffer();

	/* Make sure they are equal*/
	assertEqual(0x22, secondCall);
	assertEqual(0x33, thirdCall);
	assertEqual(0x44, fourthCall);
	assertEqual(0x55, fifthCall);
	//Make sure call count is as expected
	assertEqual(0x5, callCount);

}

void setup()
{

#if !defined(EPOXY_DUINO)
	delay(1000); // wait to prevent garbage on SERIAL_PORT_MONITOR
#endif

	SERIAL_PORT_MONITOR.begin(115200);
	while (!SERIAL_PORT_MONITOR)
		; // needed for Leonardo/Micro

	TestRunner::setTimeout(0);

	SPI.begin();

}

void loop()
{
	TestRunner::run();
}