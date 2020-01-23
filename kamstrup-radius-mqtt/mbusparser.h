#ifndef MBUSPARSER_H
#define MBUSPARSER_H

#include <vector>
#include <cstring>
#include <cmath>
#include <cstdint>

struct MeterData {
  // Active Power +/-
  uint32_t activePowerPlus = 0;
  bool activePowerPlusValid = false;
  uint32_t activePowerMinus = 0;
  bool activePowerMinusValid = false;
  uint32_t activePowerPlusL1 = 0;
  bool activePowerPlusValidL1 = false;
  uint32_t activePowerMinusL1 = 0;
  bool activePowerMinusValidL1 = false;
  uint32_t activePowerPlusL2 = 0;
  bool activePowerPlusValidL2 = false;
  uint32_t activePowerMinusL2 = 0;
  bool activePowerMinusValidL2 = false;
  uint32_t activePowerPlusL3 = 0;
  bool activePowerPlusValidL3 = false;
  uint32_t activePowerMinusL3 = 0;
  bool activePowerMinusValidL3 = false;

  // Reactive Power +/-
  uint32_t reactivePowerPlus = 0;
  bool reactivePowerPlusValid = false;
  uint32_t reactivePowerMinus = 0;
  bool reactivePowerMinusValid = false;

  // Voltage L1/L2/L3
  uint32_t voltageL1 = 0;
  bool voltageL1Valid = false;
  uint32_t voltageL2 = 0;
  bool voltageL2Valid = false;
  uint32_t voltageL3 = 0;
  bool voltageL3Valid = false;

  // Current L1/L2/L3
  uint32_t centiAmpereL1 = 0;
  bool centiAmpereL1Valid = false;
  uint32_t centiAmpereL2 = 0;
  bool centiAmpereL2Valid = false;
  uint32_t centiAmpereL3 = 0;
  bool centiAmpereL3Valid = false;

  //Energy
  uint32_t activeImportWh = 0;
  bool activeImportWhValid = false;
  uint32_t activeExportWh = 0;
  bool activeExportWhValid = false;
  uint32_t activeImportWhL1 = 0;
  bool activeImportWhValidL1 = false;
  uint32_t activeExportWhL1 = 0;
  bool activeExportWhValidL1 = false;
  uint32_t activeImportWhL2 = 0;
  bool activeImportWhValidL2 = false;
  uint32_t activeExportWhL2 = 0;
  bool activeExportWhValidL2 = false;
  uint32_t activeImportWhL3 = 0;
  bool activeImportWhValidL3 = false;
  uint32_t activeExportWhL3 = 0;
  bool activeExportWhValidL3 = false;

  uint32_t reactiveImportWh = 0;
  bool reactiveImportWhValid = false;
  uint32_t reactiveExportWh = 0;
  bool reactiveExportWhValid = false;

  //Powerfactor
  uint32_t powerFactorL1 = 0;
  bool powerFactorValidL1 = false;
  uint32_t powerFactorL2 = 0;
  bool powerFactorValidL2 = false;
  uint32_t powerFactorL3 = 0;
  bool powerFactorValidL3 = false;
  uint32_t powerFactorTotal = 0;
  bool powerFactorTotalValid = false;
  
  uint8_t listId = 0;
  size_t parseResultBufferSize = 0;
  size_t parseResultMessageSize = 0;
};

struct VectorView {
  VectorView(const std::vector<uint8_t>& data, size_t position, size_t size)
    : m_start(&data[position])
    , m_size(size)
  {
  }
  VectorView(const uint8_t* data, size_t size)
    : m_start(data)
    , m_size(size)
  {
  }
  const uint8_t& operator[](size_t pos) const { return m_start[pos]; }
  const uint8_t& front() const { return m_start[0]; }
  const uint8_t& back() const { return m_start[m_size-1]; }
  size_t size() const noexcept { return m_size; }
  size_t find(const std::vector<uint8_t>& needle) const;
private:
  const uint8_t* m_start;
  size_t m_size;
};

std::vector<VectorView> getFrames(const std::vector<uint8_t>& data);
MeterData parseMbusFrame(const VectorView& frame);

struct MbusStreamParser {
  MbusStreamParser(uint8_t * buff, size_t bufsize);
  bool pushData(uint8_t data);
  const VectorView& getFrame();
  enum BufferContent {
    COMPLETE_FRAME,
    TRASH_DATA
  };
  BufferContent getContentType() const;
private:
  uint8_t* m_buf;
  size_t m_bufsize;
  size_t m_position = 0;

  enum ParseState {
    LOOKING_FOR_START,
    LOOKING_FOR_FORMAT_TYPE,
    LOOKING_FOR_SIZE,
    LOOKING_FOR_END,
  };
  ParseState m_parseState = LOOKING_FOR_START;
  uint16_t m_messageSize = 0;
  VectorView m_frameFound;
  BufferContent m_bufferContent = TRASH_DATA;
};

#endif
