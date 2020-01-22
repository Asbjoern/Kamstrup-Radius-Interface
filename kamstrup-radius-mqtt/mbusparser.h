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

  uint32_t activeImportWh = 0;
  bool activeImportWhValid = false;
  uint32_t activeExportWh = 0;
  bool activeExportWhValid = false;

  uint32_t reactiveImportWh = 0;
  bool reactiveImportWhValid = false;
  uint32_t reactiveExportWh = 0;
  bool reactiveExportWhValid = false;

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
