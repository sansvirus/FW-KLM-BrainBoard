#pragma once

#include "rusefi_types.h"

#if EFI_BMW_MFL

class BmwMfl {
public:
	void initIfValid(brain_pin_e pin);
	void deInit();
	void onEdge(efitick_t nowNt);

	bool isOnPressed()    const { return m_on; }
	bool isIoPressed()    const { return m_io; }
	bool isPlusPressed()  const { return m_plus; }
	bool isMinusPressed() const { return m_minus; }

private:
	void processByte(uint8_t value);

	static constexpr float   kBitThresholdUs  = 250.0f;
	static constexpr float   kFrameGapUs      = 5000.0f;
	static constexpr uint8_t kIdleRaw         = 0xFF;

	static constexpr efitick_t kHighThreshNt = US2NT(kBitThresholdUs);
	static constexpr efitick_t kFrameGapNt   = US2NT(kFrameGapUs);

	brain_pin_e m_pin = Gpio::Unassigned;

	efitick_t m_prevEdgeNt = 0;
	efitick_t m_lastRiseNt = 0;

	uint8_t m_bitsByte = 0;
	uint8_t m_idx = 0;

	volatile bool m_on = false, m_io = false, m_plus = false, m_minus = false;
	volatile uint8_t m_lastRaw = 0x00;

	bool m_prevOnHit = false,  m_prevIoHit = false,  m_prevPlusHit = false,  m_prevMinusHit = false;
	bool m_onLatched = false,  m_ioLatched = false,  m_plusLatched = false,  m_minusLatched = false;
};

extern BmwMfl bmwMfl;

#endif
