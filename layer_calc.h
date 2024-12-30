#pragma once  

class LayerCalculator {  
private:  
    struct LayerData {  
        float coilLength;    // Длина катушки в мм  
        float wireWidth;     // Диаметр провода в мм  
        int16_t turnsTotal;  // Общее количество витков  
        int16_t calcLayers;  // Расчетное количество слоев  
        int16_t turnsLayer;  // Витков в слое  
        float stepSize;      // Шаг намотки  
    };  

    LayerData data;  
    LiquidCrystalCyr& lcd;  
    Winding& winding;  
    uint8_t menuPos = 0;  

    // Пункты меню калькулятора  
    enum {  
        MENU_LENGTH,   // Длина катушки  
        MENU_CALC,     // Расчет  
        MENU_RESULT,   // Результат  
        MENU_APPLY,    // Применить  
        MENU_EXIT,     // Выход  
        MENU_ITEMS_COUNT  
    };  

public:  
    LayerCalculator(LiquidCrystalCyr& lcd, Winding& winding)   
        : lcd(lcd), winding(winding) {  
        data.coilLength = 50.0f;  // 50мм по умолчанию  
        data.turnsTotal = winding.turns;  
        data.stepSize = winding.step;  
        data.wireWidth = settings.wireWidth;  
        data.calcLayers = 0;  
        data.turnsLayer = 0;  
    }  
    
    void calculate() {  
        if (data.coilLength <= 0 || data.stepSize <= 0) return;  
        
        // Расчет витков в одном слое  
        float turnsPerLayer = (data.coilLength) / data.stepSize;  
        data.turnsLayer = floor(turnsPerLayer);  
        
        // Расчет необходимого количества слоев  
        if (data.turnsLayer > 0) {  
            data.calcLayers = ceil(float(data.turnsTotal) / data.turnsLayer);  
        }  
    }  
    
    void showMenuItem() {  
        lcd.clear();  
        switch (menuPos) {  
            case MENU_LENGTH:  
                lcd.printAt_P(0, 0, PSTR("Длина катушки:"));  
                lcd.printfAt(0, 1, "%.1f мм", data.coilLength);  
                break;  
                
            case MENU_CALC:  
                lcd.printAt_P(0, 0, PSTR("Расчет слоев"));  
                lcd.printAt_P(0, 1, PSTR("Нажмите ОК"));  
                break;  
                
            case MENU_RESULT:  
                lcd.printAt_P(0, 0, PSTR("Слоев/Витков:"));  
                lcd.printfAt(0, 1, "%d/%d", data.calcLayers, data.turnsLayer);  
                break;  
                
            case MENU_APPLY:  
                lcd.printAt_P(0, 0, PSTR("Применить?"));  
                lcd.printAt_P(0, 1, PSTR("OK-да BACK-нет"));  
                break;  
                
            case MENU_EXIT:  
                lcd.printAt_P(0, 0, PSTR("Выход"));  
                lcd.printAt_P(0, 1, PSTR("Нажмите ОК"));  
                break;  
        }  
    }  
    
    bool handleButton(uint8_t button) {  
        switch (button) {  
            case BTN_UP:  
                if (menuPos > 0) menuPos--;  
                showMenuItem();  
                break;  
                
            case BTN_DOWN:  
                if (menuPos < MENU_ITEMS_COUNT-1) menuPos++;  
                showMenuItem();  
                break;  
                
            case BTN_LEFT:  
                adjustValue(-1);  
                showMenuItem();  
                break;  
                
            case BTN_RIGHT:  
                adjustValue(1);  
                showMenuItem();  
                break;  
                
            case BTN_OK:  
                return processOK();  
        }  
        return false;  
    }  
    
private:  
    void adjustValue(int8_t dir) {  
        switch (menuPos) {  
            case MENU_LENGTH:  
                data.coilLength += dir * 0.5f;  
                if (data.coilLength < 1.0f) data.coilLength = 1.0f;  
                break;  
        }  
    }  
    
    bool processOK() {  
        switch (menuPos) {  
            case MENU_CALC:  
                calculate();  
                menuPos = MENU_RESULT;  
                break;  
                
            case MENU_APPLY:  
                if (data.calcLayers > 0) {  
                    winding.layers = data.calcLayers;  
                    SaveWindings();  
                    lcd.printAt_P(0, 0, PSTR("Сохранено!"));  
                    delay(1000);  
                }  
                return true;  
                
            case MENU_EXIT:  
                return true;  
        }  
        showMenuItem();  
        return false;  
    }  
};