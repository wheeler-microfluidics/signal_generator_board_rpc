from collections import OrderedDict
import time

import numpy as np
from nadamq.command_proxy import (NodeProxy, RemoteNodeProxy,
                                  CommandRequestManager,
                                  CommandRequestManagerDebug, SerialStream)
from .requests import (REQUEST_TYPES, CommandResponse, CommandRequest,
                       CommandType)
from .protobuf_custom import DescriptionStrings


class SignalGeneratorBoard(NodeProxy):
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
        super(SignalGeneratorBoard, self).__init__(request_manager, stream)
        self._stream._serial.setDTR(False)
        time.sleep(0.5)
        self._stream._serial.setDTR(True)
        time.sleep(1)
        print 'free memory:', self.ram_free()


class RemoteSignalGeneratorBoard(RemoteNodeProxy):
    def __init__(self, forward_proxy, remote_address, debug=False,
                 timeout=None):
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
        super(RemoteSignalGeneratorBoard, self).__init__(forward_proxy,
                                                         remote_address,
                                                         request_manager)
