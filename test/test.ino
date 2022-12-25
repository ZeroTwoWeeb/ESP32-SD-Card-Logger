#define V_ADC_PIN 39
#define REF_VOLTAGE 3.3
#define VOLTAGE_CORRECTION 0.5
#define FAKTOR 0.2

float adc_voltage;

void setup()
{
    Serial.begin(115200);
    log_d("Serial started");
}

void loop()
{
    Serial.println("Voltage: ");
    Serial.println(getVoltage());
}

String getVoltage()
{
    adc_voltage = analogRead(V_ADC_PIN);
    Serial.println(adc_voltage);
    adc_voltage = (adc_voltage * 3.3) / 4096.0;
    Serial.println(adc_voltage);
    adc_voltage = adc_voltage / FAKTOR;
    Serial.println(adc_voltage);
    adc_voltage += VOLTAGE_CORRECTION;
    Serial.println(adc_voltage);
    return String(adc_voltage);
}