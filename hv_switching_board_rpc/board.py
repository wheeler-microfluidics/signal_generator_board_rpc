import sys
import time

import bitarray
from nadamq.command_proxy import (NodeProxy, CommandRequestManager,
                                  CommandRequestManagerDebug, SerialStream)
from hv_switching_board_rpc.requests import (REQUEST_TYPES, CommandResponse,
                                             CommandRequest, CommandType)


class HVSwitchingBoard(NodeProxy):
    def __init__(self, port, baudrate=115200, debug=False):
        if not debug:
            request_manager = CommandRequestManager(REQUEST_TYPES,
                                                    CommandRequest,
                                                    CommandResponse,
                                                    CommandType)
        else:
            request_manager = CommandRequestManagerDebug(REQUEST_TYPES,
                                                         CommandRequest,
                                                         CommandResponse,
                                                         CommandType)
        stream = SerialStream(port, baudrate=baudrate)
        super(HVSwitchingBoard, self).__init__(request_manager, stream)
        self._stream._serial.setDTR(False)
        time.sleep(0.5)
        self._stream._serial.setDTR(True)
        time.sleep(1)
        print 'total memory:', self.total_ram_size()
        print 'free memory:', self.ram_free()
        # Initialize local state to match power-up state _(i.e., all off)_ of
        # switching board switches.
        self.channel_states = self.channel_count * [0]

    @property
    def channel_count(self):
        return 40

    def set_state_of_all_channels(self, channel_states):
        '''
        Set the state of all channels in on the switching board.

        `channel_states`: A list of values, each either `0` or `1`.  The length
        of the list must be equal to the channel-count of the switching board.
        '''
        if len(channel_states) != self.channel_count:
            raise ValueError('`channel_states` must contain exactly %d '
                             'values.' % self.channel_count)
        max_state = max(channel_states)
        min_state = max(channel_states)
        assert(max_state == 0 or max_state == 1)
        assert(min_state == 0 or min_state == 1)
        channels_per_register = 8
        for i in range(self.channel_count / channels_per_register):
            states = channel_states[-(i * channels_per_register + 1):
                                    -((i + 1) * channels_per_register + 1):-1]
            self.spi_write(byte=ord(bitarray.bitarray(states).tobytes()))
            # Update corresponding switches in local channel state table to
            # match the newly set switch states.
            self.channel_states[-(i * channels_per_register + 1):
                                -((i + 1) * channels_per_register
                                  + 1):-1] = states
