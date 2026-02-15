#pragma once
#include "SpiDriver.h"

/**
 * @brief Базовый класс для устройства с интерфейсом SPI
 * 
 * Инкапсулирует управление линией CS и передачу данных. 
 */

class SpiDevice {
protected:
    /// @brief Ссылка на реализацию драйвера SPI для дальнейшего взаимодействия
    SpiDriver& spi;

     /**
     * @brief Активирует линию CS
     * 
     * @note Внутренний метод, используемый производными классами.
     */

    void select() {spi.csLow();}
    /**
     * @brief Деактивирует линию CS
     * 
     * @note Внутренний метод, используемый производными классами.
     */
    void deselect() {spi.csHigh();}
     /**
     * @brief Передает один байт данных через шину SPI
     * 
     * @param data Байт для передачи
     * @return Байт, принятый от устройства
     * 
     * @note Внутренний метод, используемый производными классами.
     */

    uint8_t transfer(uint8_t data) {
        return spi.transfer(data);
    }

public:
    /**
     * @brief Конструктор класса SpiDevice
     * 
     * @param driver Ссылка на реализацию драйвера SPI.
     */
    explicit SpiDevice(SpiDriver& driver) : spi(driver) {}
    virtual ~SpiDevice() = default;
};