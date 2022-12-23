#define V_ADC_PIN 39
float R1 = 30000.0;
float R2 = 7500.0;
#define REF_VOLTAGE 3.3

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
    Serial.println("Analog read:" + String(adc_voltage));
    adc_voltage = adc_voltage * (R2 / (R1 + R2));
    adc_voltage = (adc_voltage * REF_VOLTAGE) / 1024.0;
    Serial.printf("Voltage: %f", adc_voltage);
    return String(adc_voltage);
}