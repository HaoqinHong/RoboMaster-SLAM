/****************************************************************************
 *  Copyright (C) 2019 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#ifndef ROBOMASTER_HARDWARE_INTERFACE_H
#define ROBOMASTER_HARDWARE_INTERFACE_H


namespace robomaster{
/**
 * @brief Abstract class for hardware as an interface
 */
class HardwareInterface{
 public:
  HardwareInterface(){};
  virtual ~HardwareInterface() = default;
 protected:
  /**
 * @brief Hardware initialization function
 * @return False if failed, else True
 */
  virtual bool Init() = 0;
  /**
 * @brief Hardware device read function
 * @param buf Given buffer to be updated by reading
 * @param len Read data length
 * @return -1 if failed, else the read length
 */
  virtual int Read(uint8_t *buf, int len) = 0;
  /**
 * @brief Write the buffer data into hardware device to send the data
 * @param buf Given buffer to be sent
 * @param len Send data length
 * @return < 0 if failed, else the send length
 */
  virtual int Write(const uint8_t *buf, int len) = 0;
};
}
#endif //ROBOMASTER_HARDWARE_INTERFACE_H
