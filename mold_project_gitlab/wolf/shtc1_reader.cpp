#include "shtc1_reader.hpp"

#include <bitset>
#include <thread>

#include "i2c/open_close.hpp"

using namespace logging;

wolf::shtc1_reader::shtc1_reader(boost::asio::io_service &service)
    : m_logger("shtc1_reader"),
      m_timer(service),
      m_read_interval{},
      m_reader_writer(m_logger, m_file) {}

bool wolf::shtc1_reader::open(const std::string &device, const int slave_id) {
  LOGGING_LOG(m_logger, severity::normal)
      << "opening device:" << device << " using slave_id:0x" << std::hex
      << slave_id;

  if (auto file = i2c::open_close::open(device, slave_id); file.has_value()) {
    m_file = *file;
  } else {
    LOGGING_LOG(m_logger, severity::normal)
        << "opening device:" << device << " failed";
    return false;
  }

  if (!do_soft_reset_sync()) return false;
  return get_id_register();
}

void wolf::shtc1_reader::close() {
  boost::system::error_code ignore_error;
  m_timer.cancel(ignore_error);
  i2c::open_close::close(m_file);
}

bool wolf::shtc1_reader::read_async(seconds read_interval) {
  if (read_interval < m_min_read_interval) {
    LOGGING_LOG(m_logger, severity::warning)
        << "read_interval below min interval, defaulting to "
        << m_min_read_interval.count() << " seconds";
    read_interval = m_min_read_interval;
  }

  m_read_interval = read_interval;
  return read();
}

bool wolf::shtc1_reader::do_soft_reset_sync() {
  LOGGING_LOG(m_logger, severity::normal) << "doing a soft reset";
  const bool result = send_command(m_soft_reset);
  std::this_thread::sleep_for(std::chrono::milliseconds(m_wait_for_soft_reset));
  return result;
}

bool wolf::shtc1_reader::read() {
  if (!send_command(m_measurement)) {
    trigger_read();
    return false;
  }

  m_timer.expires_from_now(std::chrono::milliseconds(m_wait_for_measurement));
  m_timer.async_wait(
      [this](const boost::system::error_code &error) { handle_read(error); });
  return true;
}

void wolf::shtc1_reader::handle_read(const boost::system::error_code &error) {
  if (error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "handle_read, error: " << error.message();
    return;
  }

  read_buffer buffer;
  if (m_reader_writer.read_into_buffer(buffer)) {
    signal_read(buffer);
  }

  trigger_read();
}

void wolf::shtc1_reader::trigger_read() {
  m_timer.expires_from_now(m_read_interval);
  m_timer.async_wait([this](const boost::system::error_code &error) {
    if (error) {
      LOGGING_LOG(m_logger, severity::warning)
          << "timer async wait error: " << error.message();
      return;
    }

    read();
  });
}

bool wolf::shtc1_reader::get_id_register() {
  if (!send_command(m_id_register)) return false;
  std::array<std::uint8_t, 3> id_buffer;
  if (!m_reader_writer.read_into_buffer(id_buffer)) {
    LOGGING_LOG(m_logger, severity::error) << "could not read ID Register";
    return false;
  }

  LOGGING_LOG(m_logger, severity::verbose)
      << "ID Register: 0b" << std::bitset<8>(id_buffer[0])
      << std::bitset<8>(id_buffer[1]);
  return true;
}

bool wolf::shtc1_reader::send_command(uint16_t command) {
  std::array<std::uint8_t, 2> to_send;
  to_send[0] = (command >> 8) & 0xFF;
  to_send[1] = (command >> 0) & 0xFF;
  return m_reader_writer.write_buffer(to_send);
}