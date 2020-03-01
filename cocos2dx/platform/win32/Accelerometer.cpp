#include "Accelerometer.h"
#include "EGLView.h"
#include "Director.h"
#include <time.h>


namespace
{
	double            g_accelX = 0.0;
	double            g_accelY = 0.0;
	double            g_accelZ = 0.0;

	const double    g_accelerationStep = 0.2f;
	const double    g_minAcceleration = -1.0f;
	const double    g_maxAcceleration = 1.0f;


	template <class T>
	T CLAMP(const T val, const T minVal, const T maxVal)
	{
		CC_ASSERT(minVal <= maxVal);
		T result = val;
		if (result < minVal)
			result = minVal;
		else if (result > maxVal)
			result = maxVal;

		CC_ASSERT(minVal <= result && result <= maxVal);
		return result;
	}

	bool handleKeyDown(WPARAM wParam)
	{
		bool    sendUpdate = false;
		switch (wParam)
		{
		case VK_LEFT:
			sendUpdate = true;
			g_accelX = CLAMP(g_accelX - g_accelerationStep, g_minAcceleration, g_maxAcceleration);
			break;
		case VK_RIGHT:
			sendUpdate = true;
			g_accelX = CLAMP(g_accelX + g_accelerationStep, g_minAcceleration, g_maxAcceleration);
			break;
		case VK_UP:
			sendUpdate = true;
			g_accelY = CLAMP(g_accelY + g_accelerationStep, g_minAcceleration, g_maxAcceleration);
			break;
		case VK_DOWN:
			sendUpdate = true;
			g_accelY = CLAMP(g_accelY - g_accelerationStep, g_minAcceleration, g_maxAcceleration);
			break;
		case VK_OEM_COMMA:   // <
			sendUpdate = true;
			g_accelZ = CLAMP(g_accelZ + g_accelerationStep, g_minAcceleration, g_maxAcceleration);
			break;
		case VK_OEM_PERIOD:  // >
			sendUpdate = true;
			g_accelZ = CLAMP(g_accelZ - g_accelerationStep, g_minAcceleration, g_maxAcceleration);
			break;
		}
		return sendUpdate;
	}

	bool handleKeyUp(WPARAM wParam)
	{
		bool    sendUpdate = false;
		switch (wParam)
		{
		case VK_LEFT:
		case VK_RIGHT:
			sendUpdate = true;
			g_accelX = 0.0;
			break;
		case VK_UP:
		case VK_DOWN:
			sendUpdate = true;
			g_accelY = 0.0;
			break;
		case VK_OEM_COMMA:
		case VK_OEM_PERIOD:
			sendUpdate = true;
			g_accelZ = 0.0;
			break;
		}
		return sendUpdate;
	}

	void myAccelerometerKeyHook(UINT message, WPARAM wParam, LPARAM lParam)
	{
		cocos2d::Accelerometer* pAccelerometer = cocos2d::Director::sharedDirector()->getAccelerometer();
		bool sendUpdate = false;
		switch (message)
		{
		case WM_KEYDOWN:
			sendUpdate = handleKeyDown(wParam);
			break;
		case WM_KEYUP:
			sendUpdate = handleKeyUp(wParam);
			break;
		case WM_CHAR:
			// Deliberately empty - all handled through key up and down events
			break;
		default:
			// Not expected to get here!!
			CC_ASSERT(false);
			break;
		}

		if (sendUpdate)
		{
			const time_t    theTime = time(NULL);
			const double    timestamp = (double)theTime / 100.0;
			pAccelerometer->update(g_accelX, g_accelY, g_accelZ, timestamp);
		}
	}

	void resetAccelerometer()
	{
		g_accelX = 0.0;
		g_accelY = 0.0;
		g_accelZ = 0.0;
	}
}


NS_CC_BEGIN

Accelerometer::Accelerometer() :
	_pAccelDelegate(NULL)
{
	memset(&_obAccelerationValue, 0, sizeof(_obAccelerationValue));
}

Accelerometer::~Accelerometer()
{

}


void Accelerometer::setDelegate(AccelerometerDelegate* pDelegate)
{
	_pAccelDelegate = pDelegate;

	if (pDelegate)
	{
		EGLView::sharedOpenGLView()->setAccelerometerKeyHook(&myAccelerometerKeyHook);
	}
	else
	{
		EGLView::sharedOpenGLView()->setAccelerometerKeyHook(NULL);
		resetAccelerometer();
	}
}

void Accelerometer::update(double x, double y, double z, double timestamp)
{
	if (_pAccelDelegate)
	{
		_obAccelerationValue.x = x;
		_obAccelerationValue.y = y;
		_obAccelerationValue.z = z;
		_obAccelerationValue.timestamp = timestamp;

		_pAccelDelegate->didAccelerate(&_obAccelerationValue);
	}
}





NS_CC_END