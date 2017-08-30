#pragma once

#include <Arduino.h>
#include <Seeed_ws2812.h>
#include <time.h>
#include <vector>

#define WIOLTE_TCP	(WioLTE::SOCKET_TCP)
#define WIOLTE_UDP	(WioLTE::SOCKET_UDP)

class WioLTE
{
	/////////////////////////////////////////////////////////////////////
	// Stopwatch

private:
	class Stopwatch
	{
	private:
		unsigned long _BeginTime;
		unsigned long _EndTime;

	public:
		Stopwatch() : _BeginTime(0), _EndTime(0)
		{
		}

		void Start()
		{
			_BeginTime = millis();
			_EndTime = 0;
		}

		void Stop()
		{
			_EndTime = millis();
		}

		unsigned long ElapsedMilliseconds() const
		{
			if (_EndTime == 0) {
				return millis() - _BeginTime;
			}
			else {
				return _EndTime - _BeginTime;
			}
		}

	};

	/////////////////////////////////////////////////////////////////////
	// ArgumentParser

private:
	class ArgumentParser
	{
	public:
		std::vector< std::vector<char> > _Arguments;

	public:
		ArgumentParser();
		void Parse(const char* str);
		int Size() const;
		const char* operator[](int index) const;

	};

	/////////////////////////////////////////////////////////////////////
	// ModuleSerial

private:
	class ModuleSerial
	{
	private:
		void SerialInit() { Serial1.begin(115200); }
		bool SerialAvailable() const { return Serial1.available() >= 1 ? true : false; }
		byte SerialRead() { return Serial1.read(); }
		void SerialWrite(byte data) { Serial1.write(data); }
		void SerialWrite(const byte* data, int dataSize) { Serial1.write(data, dataSize); }

	public:
		enum WaitForResponseFlag {
			WFR_WITHOUT_DELIM		= 0x01,
			WFR_START_WITH			= 0x02,
			WFR_REMOVE_START_WITH	= 0x04,
		};

	public:
		ModuleSerial();

	private:
		std::vector<char> _LastResponse;

		void DiscardRead();
		bool WaitForAvailable(Stopwatch* sw, long timeout) const;
		int Read(byte* data, int dataSize);
		const char* ReadResponse(const char* match = NULL);

	public:
		void Init();
		void Write(const byte* data, int dataSize);
		void Write(const char* str);
		int Read(byte* data, int dataSize, long timeout);

		void WriteCommand(const char* command);
		const char* WaitForResponse(const char* waitResponse, long timeout, const char* waitPattern = NULL, WaitForResponseFlag waitPatternFlag = (WaitForResponseFlag)0);
		const char* WriteCommandAndWaitForResponse(const char* command, const char* response, long timeout);

	};

	/////////////////////////////////////////////////////////////////////
	// WioLTE

private:
	static const int MODULE_PWR_PIN = 18;		// PB2
	static const int ANT_PWR_PIN = 28;			// PB12
	static const int ENABLE_VCCB_PIN = 26;		// PB10    

	static const int PWR_KEY_PIN = 36;			// PC4 
	static const int WAKEUP_IN_PIN = 32;		// PC0
	static const int AP_READY_PIN = 33;			// PC1
	static const int WAKEUP_DISABLE_PIN = 34;	// PC2
	static const int RESET_MODULE_PIN = 35;		// PC3
	static const int STATUS_PIN = 31;			// PB15
	static const int RGB_LED_PIN = 17;			// PB1

private:
	ModuleSerial _Module;
	WS2812 _Led;

public:
	enum SocketType {
		SOCKET_TCP,
		SOCKET_UDP,
	};

private:
	bool ErrorOccured(int lineNumber, bool value);
	int ErrorOccured(int lineNumber, int value);

	bool Reset();
	bool TurnOn();

	int GetFirstIndexOfReceivedSMS();

	bool HttpSetUrl(const char* url);

public:
	WioLTE();
	void Init();
	void LedSetRGB(byte red, byte green, byte blue);
	void PowerSupplyLTE(bool on);
	void PowerSupplyGNSS(bool on);
	void PowerSupplyGrove(bool on);
	bool IsBusy() const;
	bool TurnOnOrReset();

	int GetPhoneNumber(char* number, int numberSize);
	int GetReceivedSignalStrength();
	bool GetTime(struct tm* tim);

	bool SendSMS(const char* dialNumber, const char* message);
	int ReceiveSMS(char* message, int messageSize);
	bool DeleteReceivedSMS();

	bool Activate(const char* accessPointName, const char* userName, const char* password);

	bool SyncTime(const char* host);

	int SocketOpen(const char* host, int port, SocketType type);
	bool SocketSend(int connectId, const byte* data, int dataSize);
	bool SocketSend(int connectId, const char* data);
	int SocketReceive(int connectId, byte* data, int dataSize);
	int SocketReceive(int connectId, char* data, int dataSize);
	int SocketReceive(int connectId, byte* data, int dataSize, long timeout);
	int SocketReceive(int connectId, char* data, int dataSize, long timeout);
	bool SocketClose(int connectId);

	int HttpGet(const char* url, char* data, int dataSize);
	bool HttpPost(const char* url, char* data);

};