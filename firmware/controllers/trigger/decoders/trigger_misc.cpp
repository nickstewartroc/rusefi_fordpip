/*
 * trigger_misc.cpp
 *
 *  Created on: Oct 30, 2018
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "pch.h"

#include "trigger_misc.h"
#include "trigger_universal.h"

// TT_FIAT_IAW_P8
void configureFiatIAQ_P8(TriggerWaveform * s) {
	s->initialize(FOUR_STROKE_CAM_SENSOR, SyncEdge::Rise);

	int width = 60;
	s->tdcPosition = width;

	s->addEvent720(width, TriggerValue::RISE);
	s->addEvent720(180, TriggerValue::FALL);

	s->addEvent720(180 + width, TriggerValue::RISE);
	s->addEvent720(720, TriggerValue::FALL);
	s->setTriggerSynchronizationGap(3);
}

// TT_TRI_TACH
void configureTriTach(TriggerWaveform * s) {
	s->initialize(FOUR_STROKE_CRANK_SENSOR, SyncEdge::RiseOnly);

	s->isSynchronizationNeeded = false;

	float toothWidth = 0.5;

	float engineCycle = FOUR_STROKE_ENGINE_CYCLE;

	int totalTeethCount = 135;
	float offset = 0;

	float angleDown = engineCycle / totalTeethCount * (0 + (1 - toothWidth));
	float angleUp = engineCycle / totalTeethCount * (0 + 1);
	s->addEventClamped(offset + angleDown, TriggerValue::RISE, TriggerWheel::T_PRIMARY, NO_LEFT_FILTER, NO_RIGHT_FILTER);
	s->addEventClamped(offset + angleDown + 0.1, TriggerValue::RISE, TriggerWheel::T_SECONDARY, NO_LEFT_FILTER, NO_RIGHT_FILTER);
	s->addEventClamped(offset + angleUp, TriggerValue::FALL, TriggerWheel::T_PRIMARY, NO_LEFT_FILTER, NO_RIGHT_FILTER);
	s->addEventClamped(offset + angleUp + 0.1, TriggerValue::FALL, TriggerWheel::T_SECONDARY, NO_LEFT_FILTER, NO_RIGHT_FILTER);


	addSkippedToothTriggerEvents(TriggerWheel::T_SECONDARY, s, totalTeethCount, /* skipped */ 0, toothWidth, offset, engineCycle,
			1.0 * FOUR_STROKE_ENGINE_CYCLE / 135,
			NO_RIGHT_FILTER);
}

/**
 * based on https://fordsix.com/threads/understanding-standard-and-signature-pip-thick-film-ignition.81515/
 * based on https://www.w8ji.com/distributor_stabbing.htm
 */
void configureFordPip(TriggerWaveform * s) {
    	/*
	 * Observed stock Ford 5.0 TFI Signature PIP pattern.
	 *
	 * Distributor-mounted PIP sensor:
	 * - 8 pulses per distributor revolution
	 * - 1 distributor revolution = 720 crank degrees
	 * - Normal pulse high width ~= 45 crank degrees
	 * - Normal low width ~= 45 crank degrees
	 * - Signature pulse high width ~= 31.5 crank degrees
	 * - Long low gap after signature pulse ~= 58.5 crank degrees
	 *
	 * This shape is rotated so the signature pulse FALL is at 720 degrees.
	 * That matches the EpicEFI Universal Crank table that got closest:
	 *
	 * 360-degree table:
	 *   Rise: 29.25, 74.25, 119.25, 164.25, 209.25, 254.25, 299.25, 344.25
	 *   Fall: 51.75, 96.75, 141.75, 186.75, 231.75, 276.75, 321.75, 360
	 *
	 * Converted to 720 crank degrees by multiplying by 2.
	 */

	s->initialize(FOUR_STROKE_CAM_SENSOR, SyncEdge::Both);

	/*
	 * The useful sync event is the RISE after:
	 *   short high signature pulse -> long low gap -> RISE
	 *
	 * Observed ratio from your logs:
	 *   long low / short high ~= 1.85
	 *
	 * Start with a reasonably wide window.
	 */
	s->setTriggerSynchronizationGap2(1.60f, 2.10f);

	/*
	 * Starting value only. This affects timing alignment, not whether RPM syncs.
	 * Once RPM is correct and stable, set fixed timing and adjust trigger offset
	 * with a timing light.
	 *
	 * You can also temporarily keep the existing Ford PIP value if that worked
	 * better for base timing:
	 *   s->tdcPosition = 662.5f;
	 */
	s->tdcPosition = 0;

	// Normal pulses
	s->addEventAngle(58.5f,  TriggerValue::RISE);
	s->addEventAngle(103.5f, TriggerValue::FALL);

	s->addEventAngle(148.5f, TriggerValue::RISE);
	s->addEventAngle(193.5f, TriggerValue::FALL);

	s->addEventAngle(238.5f, TriggerValue::RISE);
	s->addEventAngle(283.5f, TriggerValue::FALL);

	s->addEventAngle(328.5f, TriggerValue::RISE);
	s->addEventAngle(373.5f, TriggerValue::FALL);

	s->addEventAngle(418.5f, TriggerValue::RISE);
	s->addEventAngle(463.5f, TriggerValue::FALL);

	s->addEventAngle(508.5f, TriggerValue::RISE);
	s->addEventAngle(553.5f, TriggerValue::FALL);

	s->addEventAngle(598.5f, TriggerValue::RISE);
	s->addEventAngle(643.5f, TriggerValue::FALL);

	// Signature pulse: short high, then long low after wrap
	s->addEventAngle(688.5f, TriggerValue::RISE);
	s->addEventAngle(720.0f, TriggerValue::FALL);

}

void configureFordFoxbodyPip(TriggerWaveform * s) {
	/*
	 * Observed stock Ford 5.0 TFI Signature PIP pattern.
	 *
	 * Distributor-mounted PIP sensor:
	 * - 8 pulses per distributor revolution
	 * - 1 distributor revolution = 720 crank degrees
	 * - Normal pulse high width ~= 45 crank degrees
	 * - Normal low width ~= 45 crank degrees
	 * - Signature pulse high width ~= 31.5 crank degrees
	 * - Long low gap after signature pulse ~= 58.5 crank degrees
	 *
	 * This shape is rotated so the signature pulse FALL is at 720 degrees.
	 * That matches the EpicEFI Universal Crank table that got closest:
	 *
	 * 360-degree table:
	 *   Rise: 29.25, 74.25, 119.25, 164.25, 209.25, 254.25, 299.25, 344.25
	 *   Fall: 51.75, 96.75, 141.75, 186.75, 231.75, 276.75, 321.75, 360
	 *
	 * Converted to 720 crank degrees by multiplying by 2.
	 */

	s->initialize(FOUR_STROKE_CAM_SENSOR, SyncEdge::Rise);

	/*
	 * The useful sync event is the RISE after:
	 *   short high signature pulse -> long low gap -> RISE
	 *
	 * Observed ratio from your logs:
	 *   long low / short high ~= 1.85
	 *
	 * Start with a reasonably wide window.
	 */
	s->setTriggerSynchronizationGap2(1.60f, 2.10f);

	/*
	 * Starting value only. This affects timing alignment, not whether RPM syncs.
	 * Once RPM is correct and stable, set fixed timing and adjust trigger offset
	 * with a timing light.
	 *
	 * You can also temporarily keep the existing Ford PIP value if that worked
	 * better for base timing:
	 *   s->tdcPosition = 662.5f;
	 */
	s->tdcPosition = 0;

	// Normal pulses
	s->addEventAngle(58.5f,  TriggerValue::RISE);
	s->addEventAngle(103.5f, TriggerValue::FALL);

	s->addEventAngle(148.5f, TriggerValue::RISE);
	s->addEventAngle(193.5f, TriggerValue::FALL);

	s->addEventAngle(238.5f, TriggerValue::RISE);
	s->addEventAngle(283.5f, TriggerValue::FALL);

	s->addEventAngle(328.5f, TriggerValue::RISE);
	s->addEventAngle(373.5f, TriggerValue::FALL);

	s->addEventAngle(418.5f, TriggerValue::RISE);
	s->addEventAngle(463.5f, TriggerValue::FALL);

	s->addEventAngle(508.5f, TriggerValue::RISE);
	s->addEventAngle(553.5f, TriggerValue::FALL);

	s->addEventAngle(598.5f, TriggerValue::RISE);
	s->addEventAngle(643.5f, TriggerValue::FALL);

	// Signature pulse: short high, then long low after wrap
	s->addEventAngle(688.5f, TriggerValue::RISE);
	s->addEventAngle(720.0f, TriggerValue::FALL);
	

}

void configureFordST170(TriggerWaveform * s) {
	s->initialize(FOUR_STROKE_CAM_SENSOR, SyncEdge::RiseOnly);
	int width = 10;

	int total = s->getCycleDuration() / 8;

	s->addEventAngle(1 * total - width, TriggerValue::RISE);
	s->addEventAngle(1 * total, TriggerValue::FALL);

	s->addEventAngle(2 * total - width, TriggerValue::RISE);
	s->addEventAngle(2 * total, TriggerValue::FALL);

	s->addEventAngle(4 * total - width, TriggerValue::RISE);
	s->addEventAngle(4 * total, TriggerValue::FALL);

	s->addEventAngle(6 * total - width, TriggerValue::RISE);
	s->addEventAngle(6 * total, TriggerValue::FALL);

	s->addEventAngle(8 * total - width, TriggerValue::RISE);
	s->addEventAngle(8 * total, TriggerValue::FALL);
}

static void daihatsu(TriggerWaveform * s, int count) {
	s->initialize(FOUR_STROKE_CAM_SENSOR, SyncEdge::RiseOnly);
	s->tdcPosition = 720 - 30;

	int width = 10;

	s->addEventAngle(30 - width, TriggerValue::RISE);
	s->addEventAngle(30, TriggerValue::FALL);

	for (int i = 1;i<=count;i++) {
		s->addEventAngle(s->getCycleDuration() / count * i - width, TriggerValue::RISE);
		s->addEventAngle(s->getCycleDuration() / count * i, TriggerValue::FALL);
	}
}

void configureDaihatsu3cyl(TriggerWaveform * s) {
	daihatsu(s, 3);
	s->setTriggerSynchronizationGap(0.125);
}


void configureDaihatsu4cyl(TriggerWaveform * s) {
	daihatsu(s, 4);
	s->setTriggerSynchronizationGap(0.17);
}

void configureBarra3plus1cam(TriggerWaveform *s) {
	s->initialize(FOUR_STROKE_CAM_SENSOR, SyncEdge::RiseOnly);

	// This wheel has four teeth
	// two short gaps, and two long gaps
	// short = 60 deg
	// long = 120 deg

	{
		int offset = 60;
		int w = 5;

		s->addToothRiseFall(offset, w);

		// short gap 60 deg
		s->addToothRiseFall(offset + 60, w);

		// long gap 120 deg
		s->addToothRiseFall(offset + 180, w);
		// long gap 120 deg
		s->addToothRiseFall(offset + 300, w);

		// short gap, 60 deg back to zero/720
	}

	// sync tooth is the zero tooth, the first short gap after two long gaps
	s->setTriggerSynchronizationGap3(0, 1.6f, 2.4f);
	// previous gap should be 1.0
	s->setTriggerSynchronizationGap3(1, 0.8f, 1.2f);
}

void configureBenelli(TriggerWaveform *s) {
	s->initialize(FOUR_STROKE_CRANK_SENSOR, SyncEdge::RiseOnly);

    constexpr float magic = 15;

	float angle = 3 * magic;

	s->addEvent360(angle / 2 /* 22.5 */, TriggerValue::RISE);
	s->addEvent360(angle /*     45   */, TriggerValue::FALL);

	for (int i = 0;i< 24 - 3;i++) {
		angle += magic / 2;
		s->addEvent360(angle, TriggerValue::RISE);

		angle += magic / 2;
		s->addEvent360(angle, TriggerValue::FALL);
	}
}

void configure60degSingleTooth(TriggerWaveform *s) {
	/** @todo
	 * My approach was to utilize ::Both especially for single
	 * tooth and manual kikstarter, to be ready on both sides of blind.
	 * But unfortuneally, my experiments show me the Trigger can't
	 * become syncronized by 'last' and folowed 'first' events only.
	 * Also I observe phase-aligment mehanism is trying to consume a
	 * longer side of trigger as latest before TDC.
	 * I wish to setup SyncEdge::Both for my TT_60DEG_TOOTH after
	 * this case of scenario become work well. For now, ::Rise work
	 * well for my 60 degree trigger and both edges phase sync work
	 * as mush as expected for my engine startup. */

	s->initialize(FOUR_STROKE_CRANK_SENSOR, SyncEdge::Rise);

	s->addEvent360(300, TriggerValue::RISE);
	s->addEvent360(360, TriggerValue::FALL);

	s->tdcPosition = 60;

	s->isSynchronizationNeeded = false;
	s->useOnlyPrimaryForSync = true;
}

void configureArcticCat(TriggerWaveform *s) {
  s->initialize(TWO_STROKE, SyncEdge::Rise);

  int totalTeethCount = 24;
  float engineCycle = TWO_STROKE_CYCLE_DURATION;
  float toothWidth = 0.5;

    addSkippedToothTriggerEvents(TriggerWheel::T_PRIMARY, s, totalTeethCount, 0, toothWidth, /*offset*/0, engineCycle,
    		/*from*/ 30 + 1, /* to */ 195 + 1);


    addSkippedToothTriggerEvents(TriggerWheel::T_PRIMARY, s, totalTeethCount, 0, toothWidth, /*offset*/0, engineCycle,
    		/*from*/ 210 + 1, /* to */ NO_RIGHT_FILTER);

  s->setTriggerSynchronizationGap(2);
  int c = 9;
  for (int gapIndex = 1; gapIndex <= c; gapIndex++) {
    s->setTriggerSynchronizationGap3(gapIndex, 0.75, 1.25);
  }


}
