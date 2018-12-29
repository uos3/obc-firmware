/**
 * @ingroup gnulinux
 * @ingroup radio
 *
 * @file radio.c
 *
 * @{
 */

#include "board.h"
#include "../radio.h"

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

static pid_t tone_pid = 0;

static void tone_on()
{
  if (0 == (tone_pid = fork()))
  {
    /* Child */
    if (-1 == execve("speaker-test", "-t", "sine", "-f", "800", NULL))
    {
      perror("child process execve failed [%m]");
      return -1;
    }
  }
}

static void tone_off(void)
{
  if(0 != tone_pid)
  {
    if(0 != kill(tone_pid, SIGTERM))
    {
      kill(tone_pid, SIGKILL);
    }
    tone_pid = 0;
  }
}

void Radio_tx_msk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void end_of_tx_handler(void))
{
  (void) radio_config;
  (void) data_buffer;
  (void) data_length;

  end_of_tx_handler();
}

void Radio_tx_fsk(radio_config_t *radio_config, uint8_t *data_buffer, uint32_t data_length, void end_of_tx_handler(void))
{
  (void) radio_config;
  (void) data_buffer;
  (void) data_length;

  end_of_tx_handler();
}

void Radio_tx_morse(radio_config_t *radio_config, uint8_t *text_buffer, uint32_t text_length, void end_of_tx_handler(void))
{
  (void) radio_config;

  Packet_cw_transmit_buffer(text_buffer, text_length, tone_on, tone_off);

  end_of_tx_handler();
}

void Radio_tx_off(radio_config_t *radio_config)
{
  (void) radio_config;

  return;
}


void Radio_rx_receive(radio_config_t *radio_config, uint8_t *receive_buffer, uint32_t receive_length, void received_packet_handler(void))
{
  (void) radio_config;
  (void) receive_buffer;
  (void) receive_length;

  return;
}

void Radio_rx_off(radio_config_t *radio_config)
{
  (void) radio_config;

  return;
}

/**
 * @}
 */