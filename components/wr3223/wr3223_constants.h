#include <cstdint>

namespace esphome {
namespace wr3223 {   

// Allgemeiner Startpunkt der eigentlichen Daten in einer Antwort
constexpr int DATA_START = 3;

       class MessageControl {
            public:
        /** Start der Nachricht */
        static constexpr const uint8_t STX = 0x02;
        /** Ende der Nachricht */
        static constexpr const uint8_t ETX = 0x03;
        /** Ende der Übertragung */
        static constexpr const uint8_t EOT = 0x04;
        /** Anfrage / Anforderung */
        static constexpr const uint8_t ENQ = 0x05;
        /** Positive Rückmeldung */
        static constexpr const uint8_t ACK = 0x06;
        /** Negative Rückmeldung */
        static constexpr const uint8_t NAK = 0x15;
        };

        // allgm. startpunkt der eigentlichen daten
        

  class WR3223Commands {
    public:
    //Abtau ein, beginn Abtauung ab Verdampfertemperatur. Temperatur lesen/schreiben
    static constexpr const char *AE = "AE";

    // Abtau aus, ende Abtauung ab Verdampfertemperatur. Temperatur lesen/schreiben
    static constexpr const char *AA = "AA";

    // Abtau Zuluft lesen/schreiben 
    static constexpr const char *Az = "Az";

    // Abtauluft lesen/schreiben 
    static constexpr const char *Aa = "Aa";

    // Frost aus lesen/schreiben 
    static constexpr const char *AR = "AR";

    // Frost an Schnittstelle lesen/schreiben 
    static constexpr const char *AZ = "AZ";

    // Abtaupause lesen/schreiben 
    static constexpr const char *AP = "AP";

    // Abtaunachlauf lesen/schreiben 
    static constexpr const char *AN = "AN";

    // Frost Verzögerung lesen 
    static constexpr const char *AV = "AV";

    // Verdampfertemperatur istwert auslesen 
    static constexpr const char *T1 = "T1";

    // Kondensatortemperatur auslesen 
    static constexpr const char *T2 = "T2";

    // Aussentemperatur auslesen 
    static constexpr const char *T3 = "T3";

    // Ablufttemperatur (Raumtemperatur) 
    static constexpr const char *T4 = "T4";

    // Temperatur nach Wärmetauscher (Fortluft) lesen 
    static constexpr const char *T5 = "T5";

    // Zulufttemperatur auslesen 
    static constexpr const char *T6 = "T6";

    // Temperatur nach Solevorerwärmung lesen 
    static constexpr const char *T7 = "T7";

    // Temperatur nach Vorheizregister lesen 
    static constexpr const char *T8 = "T8";

    // aktuelle Luftstufe lesen/schreiben 
    static constexpr const char *LS = "LS";

    // Luftstufe 1 lesen/schreiben 
    static constexpr const char *L1 = "L1";

    // Luftstufe 2 lesen/schreiben 
    static constexpr const char *L2 = "L2";

    // Luftstufe 3 lesen/schreiben 
    static constexpr const char *L3 = "L3";

    // Zuluft +/- lesen/schreiben 
    static constexpr const char *LD = "LD";

    // Abluft +/- lesen/schreiben 
    static constexpr const char *Ld = "Ld";

    // Erdwärmetauscher vorhanden lesen/schreiben 
    static constexpr const char *EC = "EC";

    // Schaltpunkt Sommer stopp lesen/schreiben 
    static constexpr const char *Es = "Es";

    // Schaltpunkt Erdwärmetauscher Sommer lesen/schreiben 
    static constexpr const char *ES = "ES";

    // Schaltpunkt Erdwärmetauscher Winter lesen/schreiben 
    static constexpr const char *EW = "EW";

    // Schaltpunkt Solepumpe Ein lesen/schreiben 
    static constexpr const char *EE = "EE";

    // Schaltpunkt Solepumpe aus lesen/schreiben 
    static constexpr const char *EA = "EA";

    // Fehler lesen 
    static constexpr const char *ER = "ER";

    // Status auslesen 
    static constexpr const char *ST = "ST";

    // Status schreib byte auslesen/schreiben (nur bei PC Steuerung) 
    static constexpr const char *SW = "SW";

    // Zuluftsoll Temperatur auslesen/schreiben (nur bei PC Steuerung) 
    static constexpr const char *SP = "SP";

    // Zulufttemperatur Sollwert
    static constexpr const char *Re = "Re";

    // Raumsollwert
    static constexpr const char *Rd = "Rd";

    // Mode auslesen/schreiben (nur bei PC Steuerung) 
    static constexpr const char *MD = "MD";

    // Relais lesen 
    static constexpr const char *RL = "RL";

    // Steuerspannnung Zuluft lesen 
    static constexpr const char *UZ = "UZ";

    // Steuerspannnung Abluft lesen 
    static constexpr const char *UA = "UA";

    // Drehzahl Zuluftmotor lesen 
    static constexpr const char *NZ = "NZ";

    // Drehzahl Abluftmotor lesen 
    static constexpr const char *NA = "NA";

    // Grenzdrehzahl lesen/schreiben (*50) 
    static constexpr const char *NM = "NM";

    // Configuration auslesen 
    static constexpr const char *CN = "CN";

    // Maximale Kondensationstemperatur lesen/schreiben 
    static constexpr const char *KM = "KM";

    // Zusatzheizung frei (ausgeschaltet (0) oder freigegeben (1)) lesen/schreiben 
    static constexpr const char *ZH = "ZH";

    // Zusatzheizung Ein lesen/schreiben 
    static constexpr const char *ZE = "ZE";

    // Wärmepumpe frei (freigegeben (1) oder aus (0)) lesen/schreiben 
    static constexpr const char *WP = "WP";

    // Pausezeit für Druckabbau bei automatischer Umschaltung lesen/schreiben 
    static constexpr const char *PA = "PA";

    // Identifikation lesen 
    static constexpr const char *II = "II";

    // Messwert, RWZ aktl., Aktuelle Rückwärmzahl in % 
    static constexpr const char *RA = "RA";

    // Parameter, Delta n 1 max, Max. Drehzahlabweichung Zu-/Abluft in Stufe 1 
    static constexpr const char *D1 = "D1";

    // Parameter, Delta n 2 max, Max. Drehzahlabweichung Zu-/Abluft in Stufe 2 
    static constexpr const char *D2 = "D2";

    // Parameter, Delta n 3 max, Max. Drehzahlabweichung Zu-/Abluft in Stufe 3 
    static constexpr const char *D3 = "D3";

    // Parameter, ERDluft +S1, Drehzahlerhöhung Zuluftventilator Stufe 1, wenn Erdwärmetauscher ein (0% bis 40%) 
    static constexpr const char *E1 = "E1";

    // Parameter, ERDluft +S2, Drehzahlerhöhung Zuluftventilator Stufe 2, wenn Erdwärmetauscher ein (0% bis 40%) 
    static constexpr const char *E2 = "E2";

    // Parameter, ERDluft +S3, Drehzahlerhöhung Zuluftventilator Stufe 3, wenn Erdwärmetauscher ein (0% bis 40%) 
    static constexpr const char *E3 = "E3";

    // Parameter, LuflREDUK, Luftwechsel um 3% reduziert ab Außentemp. ...°C (-20°C bis +10°C) 
    static constexpr const char *LR = "LR";

    // Parameter Solar max 
    static constexpr const char *SM = "SM";

    // Messwert Solar Nutzen (Stunden) 
    static constexpr const char *SN = "SN";

    // Parameter Delta T Aus Temperaturdifferenz zwischen Speicher u. Kollektor 
    static constexpr const char *DA = "DA";

    // Parameter Delta T Ein Temperaturdifferenz zwischen Speicher u. Kollektor 
    static constexpr const char *DE = "DE";

    // Parameter Unterstuetzungsgeblaese bei Luftstufe 1 bei EWT 
    static constexpr const char *S1 = "S1";

    // Parameter Unterstuetzungsgeblaese bei Luftstufe 2 bei EWT 
    static constexpr const char *S2 = "S2";

    // Parameter Unterstuetzungsgeblaese bei Luftstufe 3 bei EWT 
    static constexpr const char *S3 = "S3";

    // Parameter Warmwasser Sollwert 
    static constexpr const char *WS = "WS";

    // Parameter EVU Sperre 
    static constexpr const char *Tf = "Tf";

    // Status auslesen 
    static constexpr const char *Ta = "Ta";
  }; 
}
}