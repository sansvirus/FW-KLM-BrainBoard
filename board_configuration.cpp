#include "pch.h"
#include "board_overrides.h"

// PE3 is error LED, configured in board.mk
Gpio getCommsLedPin() {
	return Gpio::E4;
}

Gpio getRunningLedPin() {
	return Gpio::E5;
}

Gpio getWarningLedPin() {
	return Gpio::E6;
}

static void setInjectorPins() {
engineConfiguration->injectionPins[0] = Gpio::E1;
engineConfiguration->injectionPins[0] = Gpio::E0;
engineConfiguration->injectionPins[0] = Gpio::B9;
engineConfiguration->injectionPins[0] = Gpio::B8;
engineConfiguration->injectionPins[0] = Gpio::B7;
engineConfiguration->injectionPins[0] = Gpio::B6;
engineConfiguration->injectionPins[0] = Gpio::B5;
engineConfiguration->injectionPins[0] = Gpio::B4;
}

static void setIgnitionPins() {
engineConfiguration->ignitionPins[0] = Gpio::G2;
engineConfiguration->ignitionPins[0] = Gpio::G3;
engineConfiguration->ignitionPins[0] = Gpio::G4;
engineConfiguration->ignitionPins[0] = Gpio::G5;
engineConfiguration->ignitionPins[0] = Gpio::G6;
engineConfiguration->ignitionPins[0] = Gpio::G7;
engineConfiguration->ignitionPins[0] = Gpio::G8;
engineConfiguration->ignitionPins[0] = Gpio::C7;
}

static void setupDefaultSensorInputs() {
engineConfiguration->clt.config.bias_resistor = 2700;
engineConfiguration->iat.config.bias_resistor = 2700;
}

static void setupVbatt() {
	// 5.6k high side/10k low side = 1.56 ratio divider
	engineConfiguration->analogInputDividerCoefficient = 1.56f;

	// 82k high side/10k low side = 9.2
	engineConfiguration->vbattDividerCoeff = (92.0f / 10.0f);

	// Battery sense on PA7
	engineConfiguration->vbattAdcChannel = EFI_ADC_7;

	engineConfiguration->adcVcc = 3.3f;
}

static void setupEtb() {
	// TLE9201 driver
	// This chip has three control pins:
	// DIR - sets direction of the motor
	// PWM - pwm control (enable high, coast low)
	// DIS - disables motor (enable low)

	// Throttle #1
	// PWM pin
	engineConfiguration->etbIo[0].controlPin = Gpio::D12;
	// DIR pin
	engineConfiguration->etbIo[0].directionPin1 = Gpio::D10;
	// Disable pin
	engineConfiguration->etbIo[0].disablePin = Gpio::D11;

	// Throttle #2
	// PWM pin
	engineConfiguration->etbIo[1].controlPin = Gpio::D13;
	// DIR pin
	engineConfiguration->etbIo[1].directionPin1 = Gpio::D9;
	// Disable pin
	engineConfiguration->etbIo[1].disablePin = Gpio::D8;

	// we only have pwm/dir, no dira/dirb
	engineConfiguration->etb_use_two_wires = false;
}

static void setupDefaultSensorInputs() {
	// trigger inputs
	// Digital channel 1 as default - others not set
	engineConfiguration->triggerInputPins[0] = Gpio::E11;
	engineConfiguration->camInputs[0] = Gpio::Unassigned;

	engineConfiguration->triggerInputPins[1] = Gpio::Unassigned;


	engineConfiguration->clt.adcChannel = Gpio::Unassigned;
	engineConfiguration->iat.adcChannel = Gpio::Unassigned;
	engineConfiguration->tps1_1AdcChannel = Gpio::Unassigned;
	engineConfiguration->map.sensor.hwChannel = Gpio::Unassigned;

    // see also enableAemXSeries
	// pin #28 WBO AFR "Analog Volt 10"
	engineConfiguration->afr.hwChannel = Gpio::Unassigned;
}

static void setupSdCard() {
	engineConfiguration->sdCardSpiDevice = SPI_DEVICE_3;
	engineConfiguration->sdCardCsPin = Gpio::D2;

	engineConfiguration->is_enabled_spi_3 = true;
	engineConfiguration->spi3sckPin = Gpio::C10;
	engineConfiguration->spi3misoPin = Gpio::C11;
	engineConfiguration->spi3mosiPin = Gpio::C12;

	engineConfiguration->is_enabled_spi_5 = true;
	engineConfiguration->spi5sckPin = Gpio::F7;
	engineConfiguration->spi5misoPin = Gpio::F8;
	engineConfiguration->spi5mosiPin = Gpio::F9;
}
static void customBoardDefaultConfiguration() {
	setInjectorPins();
	setIgnitionPins();
	setupSdCard();
	setupDefaultSensorInputs();
	setupEtb();
	setupVbatt();
	
	engineConfiguration->canTxPin = Gpio::D1;
	engineConfiguration->canRxPin = Gpio::D0;
	engineConfiguration->can2RxPin = Gpio::B12;
	engineConfiguration->can2TxPin = Gpio::B13;
	engineConfiguration->lps25BaroSensorScl = Gpio::B10;
	engineConfiguration->lps25BaroSensorSda = Gpio::B11;

}
void setup_custom_board_overrides() {
    custom_board_DefaultConfiguration = customBoardDefaultConfiguration;
}

