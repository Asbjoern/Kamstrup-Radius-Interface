#include "mbusparser.h"
#include <cassert>

size_t VectorView::find(const std::vector<uint8_t>& needle) const
{
  for (const uint8_t* it = m_start; it < (m_start+m_size-needle.size()); ++it) {
    if (memcmp(it, &needle[0], needle.size()) == 0) {
      return it-m_start;
    }
  }
  return -1;
}

uint32_t getObisValue(const VectorView& frame,
                      uint8_t codeA,
                      uint8_t codeB,
                      uint8_t codeC,
                      uint8_t codeD,
                      uint8_t codeE,
                      uint8_t codeF,
                      uint8_t size,
                      bool & success)
{
  success = false;
  std::vector<uint8_t> theObis = { 0x09, 0x06, codeA, codeB, codeC, codeD, codeE, codeF };
  int indexOfData = frame.find(theObis);
  if (indexOfData >= 0) {
    //std::cout << "IndexOfActivePower: " << indexOfData << std::endl;
    const uint8_t * theBytes = &(frame[0]) + indexOfData + theObis.size() + 1;
    // for (int i = 0; i < 20; ++i) {
    //   std::cout << "Byte " << std::hex << i << ": " << (int)theBytes[i] << std::dec << std::endl;
    // }
    if (size == 2) {
      success = true;
      return
        (uint32_t)theBytes[0] << (8*1) |
        (uint32_t)theBytes[1];
    } else if (size == 3) {
      success = true;
      return
        (uint32_t)theBytes[0] << (8*2) |
        (uint32_t)theBytes[1] << (8*1) |
        (uint32_t)theBytes[2];
    } else if (size == 4) {
      success = true;
      return
        (uint32_t)theBytes[0] << (8*3) |
        (uint32_t)theBytes[1] << (8*2) |
        (uint32_t)theBytes[2] << (8*1) |
        (uint32_t)theBytes[3];
    }
  }
  return 0;
}

enum PowerType {
  ACTIVE_IMPORT,
  ACTIVE_EXPORT,
  REACTIVE_IMPORT,
  REACTIVE_EXPORT,
  ACTIVE_POWER_PLUS,
  ACTIVE_POWER_MINUS,
  REACTIVE_POWER_PLUS,
  REACTIVE_POWER_MINUS,
  VOLTAGE_L1,
  VOLTAGE_L2,
  VOLTAGE_L3,
  CURRENT_L1,
  CURRENT_L2,
  CURRENT_L3,
  ACTIVE_POWER_PLUS_L1,
  ACTIVE_POWER_PLUS_L2,
  ACTIVE_POWER_PLUS_L3,
  POWER_FACTOR_L1,
  POWER_FACTOR_L2,
  POWER_FACTOR_L3,
  POWER_FACTOR,
  ACTIVE_POWER_MINUS_L1,
  ACTIVE_POWER_MINUS_L2,
  ACTIVE_POWER_MINUS_L3,
  ACTIVE_EXPORT_L1,
  ACTIVE_EXPORT_L2,
  ACTIVE_EXPORT_L3,
  ACTIVE_IMPORT_L1,
  ACTIVE_IMPORT_L2,
  ACTIVE_IMPORT_L3
};

uint32_t getPower(const VectorView& frame,
                  PowerType type,
                  bool& success)
{
  switch (type) {
    case ACTIVE_IMPORT:        return getObisValue(frame, 1, 1, 1, 8, 0, 0xff, 4, success);
    case ACTIVE_EXPORT:        return getObisValue(frame, 1, 1, 2, 8, 0, 0xff, 4, success);
    case REACTIVE_IMPORT:      return getObisValue(frame, 1, 1, 3, 8, 0, 0xff, 4, success);
    case REACTIVE_EXPORT:      return getObisValue(frame, 1, 1, 4, 8, 0, 0xff, 4, success);
    case ACTIVE_POWER_PLUS:    return getObisValue(frame, 1, 1, 1, 7, 0, 0xff, 4, success);
    case ACTIVE_POWER_MINUS:   return getObisValue(frame, 1, 1, 2, 7, 0, 0xff, 4, success);
    case REACTIVE_POWER_PLUS:  return getObisValue(frame, 1, 1, 3, 7, 0, 0xff, 4, success);
    case REACTIVE_POWER_MINUS: return getObisValue(frame, 1, 1, 4, 7, 0, 0xff, 4, success);
    case VOLTAGE_L1:           return getObisValue(frame, 1, 1, 32, 7, 0, 0xff, 2, success);
    case VOLTAGE_L2:           return getObisValue(frame, 1, 1, 52, 7, 0, 0xff, 2, success);
    case VOLTAGE_L3:           return getObisValue(frame, 1, 1, 72, 7, 0, 0xff, 2, success);
    case CURRENT_L1:           return getObisValue(frame, 1, 1, 31, 7, 0, 0xff, 4, success);
    case CURRENT_L2:           return getObisValue(frame, 1, 1, 51, 7, 0, 0xff, 4, success);
    case CURRENT_L3:           return getObisValue(frame, 1, 1, 71, 7, 0, 0xff, 4, success);
    case ACTIVE_POWER_PLUS_L1: return getObisValue(frame, 1, 1, 21, 7, 0, 0xff, 4, success);
    case ACTIVE_POWER_PLUS_L2: return getObisValue(frame, 1, 1, 41, 7, 0, 0xff, 4, success);
    case ACTIVE_POWER_PLUS_L3: return getObisValue(frame, 1, 1, 61, 7, 0, 0xff, 4, success);
    case POWER_FACTOR_L1:      return getObisValue(frame, 1, 1, 33, 7, 0, 0xff, 2, success);
    case POWER_FACTOR_L2:      return getObisValue(frame, 1, 1, 53, 7, 0, 0xff, 2, success);
    case POWER_FACTOR_L3:      return getObisValue(frame, 1, 1, 73, 7, 0, 0xff, 2, success);
    case POWER_FACTOR:         return getObisValue(frame, 1, 1, 13, 7, 0, 0xff, 2, success); 
    case ACTIVE_POWER_MINUS_L1:return getObisValue(frame, 1, 1, 22, 7, 0, 0xff, 4, success);
    case ACTIVE_POWER_MINUS_L2:return getObisValue(frame, 1, 1, 42, 7, 0, 0xff, 4, success);
    case ACTIVE_POWER_MINUS_L3:return getObisValue(frame, 1, 1, 62, 7, 0, 0xff, 4, success); 
    case ACTIVE_EXPORT_L1:     return getObisValue(frame, 1, 1, 22, 8, 0, 0xff, 4, success);
    case ACTIVE_EXPORT_L2:     return getObisValue(frame, 1, 1, 42, 8, 0, 0xff, 4, success);
    case ACTIVE_EXPORT_L3:     return getObisValue(frame, 1, 1, 62, 8, 0, 0xff, 4, success);
    case ACTIVE_IMPORT_L1:     return getObisValue(frame, 1, 1, 21, 8, 0, 0xff, 4, success);
    case ACTIVE_IMPORT_L2:     return getObisValue(frame, 1, 1, 41, 8, 0, 0xff, 4, success);
    case ACTIVE_IMPORT_L3:     return getObisValue(frame, 1, 1, 61, 8, 0, 0xff, 4, success);
  default: return 0;
  }
}

MeterData parseMbusFrame(const VectorView& frame)
{
  MeterData result;
  unsigned int frameFormat = frame[1] & 0xF0;
  size_t messageSize = ((frame[1] & 0x0F) << 8) | frame[2];
  result.parseResultBufferSize = frame.size();
  result.parseResultMessageSize = messageSize;
  std::vector<uint8_t> needle = { 0xff, 0x80, 0x00, 0x00 };
  size_t dateTimeEnd = frame.find(needle);
  if (dateTimeEnd > 0) {
    result.listId = (frame[dateTimeEnd+5] & 0xF0) >> 4;
  }

  if (frame.front() == 0x7E && frame.back() == 0x7E) {
    if (frameFormat == 0xA0) {
      // TODO: Parse header
      // TODO: Parse datetime
      // TODO: Parse elements sequentially
      result.activePowerPlus = getPower(frame, ACTIVE_POWER_PLUS, result.activePowerPlusValid);
      result.activePowerMinus = getPower(frame, ACTIVE_POWER_MINUS, result.activePowerMinusValid);
      result.reactivePowerPlus = getPower(frame, REACTIVE_POWER_PLUS, result.reactivePowerPlusValid);
      result.reactivePowerMinus = getPower(frame, REACTIVE_POWER_MINUS, result.reactivePowerMinusValid);
      result.centiAmpereL1 = getPower(frame, CURRENT_L1, result.centiAmpereL1Valid);
      result.centiAmpereL2 = getPower(frame, CURRENT_L2, result.centiAmpereL2Valid);
      result.centiAmpereL3 = getPower(frame, CURRENT_L3, result.centiAmpereL3Valid);
      result.voltageL1 = getPower(frame, VOLTAGE_L1, result.voltageL1Valid);
      result.voltageL2 = getPower(frame, VOLTAGE_L2, result.voltageL2Valid);
      result.voltageL3 = getPower(frame, VOLTAGE_L3, result.voltageL3Valid);
      result.activeImportWh = getPower(frame, ACTIVE_IMPORT, result.activeImportWhValid)*10;
      result.activeExportWh = getPower(frame, ACTIVE_EXPORT, result.activeExportWhValid)*10;
      result.reactiveImportWh = getPower(frame, REACTIVE_IMPORT, result.reactiveImportWhValid)*10;
      result.reactiveExportWh = getPower(frame, REACTIVE_EXPORT, result.reactiveExportWhValid)*10;
      result.activePowerPlusL1 = getPower(frame, ACTIVE_POWER_PLUS_L1, result.activePowerPlusValidL1);
      result.activePowerPlusL2 = getPower(frame, ACTIVE_POWER_PLUS_L2, result.activePowerPlusValidL2);
      result.activePowerPlusL3 = getPower(frame, ACTIVE_POWER_PLUS_L3, result.activePowerPlusValidL3);
      result.activePowerMinusL1 = getPower(frame, ACTIVE_POWER_MINUS_L1, result.activePowerMinusValidL1);
      result.activePowerMinusL2 = getPower(frame, ACTIVE_POWER_MINUS_L2, result.activePowerMinusValidL2);
      result.activePowerMinusL3 = getPower(frame, ACTIVE_POWER_MINUS_L3, result.activePowerMinusValidL3);
      result.powerFactorL1 = getPower(frame, POWER_FACTOR_L1, result.powerFactorValidL1);
      result.powerFactorL2 = getPower(frame, POWER_FACTOR_L2, result.powerFactorValidL2);
      result.powerFactorL3 = getPower(frame, POWER_FACTOR_L3, result.powerFactorValidL3);
      result.powerFactorTotal = getPower(frame, POWER_FACTOR, result.powerFactorTotalValid);
      result.activeImportWhL1 = getPower(frame, ACTIVE_IMPORT_L1, result.activeImportWhValidL1);
      result.activeImportWhL2 = getPower(frame, ACTIVE_IMPORT_L2, result.activeImportWhValidL2);
      result.activeImportWhL3 = getPower(frame, ACTIVE_IMPORT_L3, result.activeImportWhValidL3);
      result.activeExportWhL1 = getPower(frame, ACTIVE_EXPORT_L1, result.activeExportWhValidL1);
      result.activeExportWhL2 = getPower(frame, ACTIVE_EXPORT_L2, result.activeExportWhValidL2);
      result.activeExportWhL3 = getPower(frame, ACTIVE_EXPORT_L3, result.activeExportWhValidL3);
    }
  }
  return result;
}

MbusStreamParser::MbusStreamParser(uint8_t* buf, size_t bufsize)
  : m_buf(buf)
  , m_bufsize(bufsize)
  , m_frameFound(nullptr, 0)
{
}

bool MbusStreamParser::pushData(uint8_t data)
{
  if (m_position >= m_bufsize) {
    // Reached end of buffer
    m_position = 0;
    m_parseState = LOOKING_FOR_START;
    m_messageSize = 0;
    m_bufferContent = TRASH_DATA;
    m_frameFound = VectorView(m_buf, m_bufsize);
    return true;
  }
  switch (m_parseState) {
  case LOOKING_FOR_START:
    m_buf[m_position++] = data;
    if (data == 0x7E) {
      // std::cout << "Found frame start. Pos=" << m_position << std::endl;
      m_parseState = LOOKING_FOR_FORMAT_TYPE;
    }
    break;
  case LOOKING_FOR_FORMAT_TYPE:
    m_buf[m_position++] = data;
    assert(m_position > 1);
    if ((data & 0xF0) == 0xA0) {
      // std::cout << "Found frame format type (pos=" << m_position << "): " << std::hex << (unsigned)data << std::dec << std::endl;
      m_parseState = LOOKING_FOR_SIZE;
      if (m_position-2 > 0) {
        m_bufferContent = TRASH_DATA;
        m_frameFound = VectorView(m_buf, m_position-2);
        return true;
      }
    } else if (data == 0x7E) {
      // std::cout << "Found frame start instead of format type: " << std::hex << (unsigned)data << std::dec << std::endl;
      m_bufferContent = TRASH_DATA;
      m_frameFound = VectorView(m_buf, m_position-1);
      return true;
    } else {
      m_parseState = LOOKING_FOR_START;
    }
    break;
  case LOOKING_FOR_SIZE:
    assert(m_position > 0);
    // Move start of frame to start of buffer
    m_buf[0] = 0x7E;
    m_buf[1] = m_buf[m_position-1];
    m_messageSize = ((m_buf[m_position-1] & 0x0F) << 8) | data;
    // std::cout << "Message size is: " << m_messageSize << ". now looking for end" << std::endl;
    m_buf[2] = data;
    m_position = 3;
    m_parseState = LOOKING_FOR_END;
    break;
  case LOOKING_FOR_END:
    m_buf[m_position++] = data;
    if (m_position == (m_messageSize+2)) {
      if (data == 0x7E) {
        m_bufferContent = COMPLETE_FRAME;
        m_frameFound = VectorView(m_buf, m_position);
        m_parseState = LOOKING_FOR_START;
        m_position = 0;
        // std::cout << "Found end. Returning complete frame. Setting position=0" << std::endl;
        return true;
      } else {
        // std::cout << "Unexpected byte at end position: " << std::hex << (unsigned)data << std::dec << std::endl;
        m_parseState = LOOKING_FOR_START;
      }
    }
    break;
  }
  return false;
}

MbusStreamParser::BufferContent MbusStreamParser::getContentType() const
{
  return m_bufferContent;
}

const VectorView& MbusStreamParser::getFrame()
{
  return m_frameFound;
}
