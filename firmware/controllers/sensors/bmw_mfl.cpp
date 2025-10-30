#include "pch.h"

#include "bmw_mfl.h"
#include "ext/rusefi/firmware/hw_layer/digital_input/digital_input_exti.h"

#if EFI_BMW_MFL

static constexpr uint8_t DELTA_ON    = 0x48;
static constexpr uint8_t DELTA_IO    = 0x12;
static constexpr uint8_t DELTA_PLUS  = 0x24;
static constexpr uint8_t DELTA_MINUS = 0x01;

BmwMfl bmwMfl;

#if EFI_PROD_CODE
static void bmwMflEdgeCallback(void* arg, efitick_t nowNt) {
	reinterpret_cast<BmwMfl*>(arg)->onEdge(nowNt);
}
#endif

void BmwMfl::initIfValid(brain_pin_e pin) {
	if (!isBrainPinValid(pin)) {
		return;
	}

#if EFI_PROD_CODE
	if (efiExtiEnablePin("BMW MFL", pin, PAL_EVENT_MODE_BOTH_EDGES, bmwMflEdgeCallback, reinterpret_cast<void*>(this)) < 0) {
		return;
	}
#endif

	m_pin = pin;
	m_idx = 0;
	m_prevEdgeNt = 0;
	m_lastRiseNt = 0;

	m_bitsByte = 0;
	m_on = m_io = m_plus = m_minus = false;

	m_prevOnHit = m_prevIoHit = m_prevPlusHit = m_prevMinusHit = false;
	m_onLatched = m_ioLatched = m_plusLatched = m_minusLatched = false;
}

void BmwMfl::onEdge(efitick_t nowNt) {
	const bool levelHighNow = efiReadPin(m_pin);

	const efitick_t dt = nowNt - m_prevEdgeNt;
	m_prevEdgeNt = nowNt;

	if (dt > kFrameGapNt) {
		m_idx = 0;
		m_bitsByte = 0;
	}

	if (levelHighNow) {
		m_lastRiseNt = nowNt;
		return;
	}

	const efitick_t highNt = nowNt - m_lastRiseNt;
	const uint8_t bit = (highNt > kHighThreshNt) ? 1 : 0;

	if (m_idx < 8) {
		m_bitsByte = (m_bitsByte << 1) | bit;
		m_idx++;
	}

	if (m_idx == 8) {
		processByte(m_bitsByte);
		m_idx = 0;
		m_bitsByte = 0;
	}
}

void BmwMfl::processByte(uint8_t value) {
	const uint8_t delta = (uint8_t)(kIdleRaw ^ value);

	const bool onHit    = (delta & DELTA_ON)    == DELTA_ON;
	const bool ioHit    = (delta & DELTA_IO)    == DELTA_IO;
	const bool plusHit  = (delta & DELTA_PLUS)  == DELTA_PLUS;
	const bool minusHit = (delta & DELTA_MINUS) == DELTA_MINUS;

	const bool onRise    = onHit    && !m_prevOnHit;    m_prevOnHit    = onHit;
	const bool ioRise    = ioHit    && !m_prevIoHit;    m_prevIoHit    = ioHit;
	const bool plusRise  = plusHit  && !m_prevPlusHit;  m_prevPlusHit  = plusHit;
	const bool minusRise = minusHit && !m_prevMinusHit; m_prevMinusHit = minusHit;

	if (engineConfiguration->bmwMflOnLatch) {
		if (onRise) m_onLatched = !m_onLatched;
		m_on = m_onLatched;
	} else {
		m_on = onHit;
	}

	if (engineConfiguration->bmwMflIoTypeLatch) {
		if (ioRise) m_ioLatched = !m_ioLatched;
		m_io = m_ioLatched;
	} else {
		m_io = ioHit;
	}

	if (engineConfiguration->bmwMflPlusTypeLatch) {
		if (plusRise) m_plusLatched = !m_plusLatched;
		m_plus = m_plusLatched;
	} else {
		m_plus = plusHit;
	}

	if (engineConfiguration->bmwMflMinusTypeLatch) {
		if (minusRise) m_minusLatched = !m_minusLatched;
		m_minus = m_minusLatched;
	} else {
		m_minus = minusHit;
	}
}

void BmwMfl::deInit() {
#if EFI_PROD_CODE
	if (isBrainPinValid(m_pin)) {
		efiExtiDisablePin(m_pin);
	}
#endif
}

#endif
