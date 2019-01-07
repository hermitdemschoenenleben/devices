from collections import namedtuple
from .card import TBusCard

class FrequencyControlCard(TBusCard):
    """
    Memory: 1 point is about 36.3636e-6 seconds
    """
    OUTPUT_MODES = namedtuple('OUTPUT_MODES', ['offset', 'offset_pid', 'offset_scan'])(0, 1, 2)
    MEMORY_SOURCES = namedtuple('MEMORY_SOURCES', [
        'pid_out', 'slow_in', 'counter',
        'demodulation', 'fast_in', 'quad', 'hex'
    ])(
        lambda idx: idx + 40,
        lambda idx: idx + 48,
        lambda idx: idx + 52,
        55,
        56,
        57,
        58
    )

    def convert_count(self, cnt, gate_time, beat_divider, beat_divider2_enabled):
        return (
            cnt * 0.5 / gate_time
                * beat_divider
                * (2 if beat_divider2_enabled else 1)
        )
    
    def convert_to_count(self, freq, gate_time, beat_divider, beat_divider2_enabled):
        resolution = (0.5 / gate_time * beat_divider * (2 if beat_divider2_enabled else 1)) / 256
        print('resolution', resolution, gate_time, beat_divider, beat_divider2_enabled)
        print(freq / resolution)
        return freq / resolution

    def get_count(self, channel):
        assert channel in [0, 1, 2], 'invalid channel value'
        cnt = self.get_int('Count%d' % (channel + 1))
        return self.convert_count(
            cnt, self.get_gate_time(), self.get_beat_divider(channel),
            self.get_beat_divider2_enabled(channel)
        )

    def get_slow(self, channel):
        assert channel in [0, 1, 2, 3], 'invalid channel value'
        return self.get_int('SlowIn%d' % (channel + 1))

    def set_gate_time(self, value):
        """
        Set gate time in seconds.
        """
        self.set_int('GateTime', value / (1e-6))
        self.queue_register('Set Gate Time')
    
    def get_gate_time(self):
        gate_time = self.get_int('GateTime') * 1e-6
        assert gate_time > 0, 'gate time not set!'
        return gate_time
    
    def set_beat_divider(self, channel, value):
        assert value in [2, 4, 8], 'invalid divider'
        assert channel in [0, 1, 2], 'invalid channel value'
        self.set_int('Beat Divider %d' % (channel + 1), {
            2: 3,
            4: 1,
            8: 0
        }[value])
        self.queue_register('Set Beat Divider')
    
    def get_beat_divider(self, channel):
        assert channel in [0, 1, 2], 'invalid channel value'
        return {
            0: 8,
            1: 4,
            2: 4,
            3: 2
        }[self.get_int('Beat Divider %d' % (channel + 1))]
    
    def set_beat_divider2_enabled(self, channel, value):
        assert channel in [0, 1, 2], 'invalid channel value'
        self.set_bool('BeatDiv2_%d' % (channel + 1), value)
        self.queue_register('Set Beat Divider')
    
    def get_beat_divider2_enabled(self, channel):
        assert channel in [0, 1, 2], 'invalid channel value'
        return self.get_bool('BeatDiv2_%d' % (channel + 1))
    
    def set_memory_source(self, value):
        """
        Which variable should be stored in memory?
        Allowed values are those in `MEMORY_SOURCES`.
        Example:

            fctl.set_memory_source(fctl.MEMORY_SOURCES.counter(1))

        """
        assert isinstance(value, int), 'value has to be integer. Did you forget to specify a channel?'
        self.set_int('Memory source', value)
        self.queue_register('Set Memory Settings')
    
    def set_memory_enabled(self, value):
        self.set_bool('Memory enable', value)
        self.queue_register('Set Memory Settings')
    
    def set_scan_amplitude(self, value):
        """
        Sets scan amplitude. Allowed values are [-1, 1].
        """
        value *= 32767
        self.set_int('Scan amplitude', value)
        self.queue_register('Set Scan Amplitude')
    
    def set_scan_rate(self, value):
        """
        Sets scan rate. A value of 0 corresponds to a period of 655us, a value of
        15 corresponds to 10.7 seconds.
        """
        self.set_int('Scan rate', value)
        self.queue_register('Set Scan Rate')
    
    def set_fast_offset(self, channel, value):
        """
        Sets offset of a fast output. Maximum value is 1, minimum value -1.
        """
        assert channel in [0, 1, 2, 3], 'invalid channel'
        value = (value + 1) * 8191.5
        self.set_int('FastOffset%d' % (channel + 1), value)
        self.queue_register('Set Fast Offset%d' % (channel + 1))
    
    def set_channel_mode_fast(self, channel, mode):
        """
        Set output mode of a fast channel.
        `mode` has to be one of `OUTPUT_MODES`.
        """
        assert channel in [0, 1, 2, 3], 'invalid channel'
        self.set_int('ChannelMode_fast%d' % (channel + 1), mode)
        self.queue_register('Set Channel Mode')
    
    def set_channel_mode_slow(self, channel, mode):
        """
        Set output mode of a slow channel.
        `mode` has to be one of `OUTPUT_MODES`.
        """
        assert channel in [0, 1, 2, 3], 'invalid channel'
        self.set_int('ChannelMode_slow%d' % (channel + 1), mode)
        self.queue_register('Set Channel Mode')
    
    def set_fifo_enabled(self, value):
        self.set_bool('Memory FIFO enable', value)
        self.queue_register('Set Memory Settings')
    
    def set_fifo_timebase_multiplier(self, value):
        assert 0 <= value <= 8191, 'invalid value'
        self.set_int('FifoTimebaseMultiplier', value)
        self.queue_register('Set FIFO counter options')
    
    def set_fifo_counter_shift(self, value):
        assert 0 <= 0 <= 7, 'invalid value' 
        self.set_int('FifoCounterShift', value)
        self.queue_register('Set FIFO counter options')
    
    def read_and_get_memory_block(self, addresses=range(1024)):
        """
        Reads a block of memory or the complete memory.
        """
        data = []

        cards = (self.name,) * len(addresses)
        registers = ('Read Memory Data',) * len(addresses)

        def before_each_send(i):
            self.set_int('Memory address', addresses[i])
        
        def after_each_read(i):
            data.append(self.get_int('Memory data'))

        self.stack.register_to_bus_multi(cards, registers,
                                         before_each_send=before_each_send,
                                         after_each_read=after_each_read)

        return data

    def read_and_get_memory_address(self, address):
        """
        Read one number at `address` from memory.
        """
        self.set_int('Memory address', address)
        self.register_to_bus('Read Memory Data')
        return self.get_int('Memory data')
    
    def read_counter(self, channel):
        assert channel in [0, 1, 2], 'invalid channel value'
        self.register_to_bus('GetCount%d' % (channel + 1))

    def read_slow(self, channel):
        assert channel in [0, 1, 2, 3], 'invalid channel value'
        self.register_to_bus('GetSlowIn%d' % (channel + 1))
    
    def read_and_get_fifo_status(self):
        self.register_to_bus('Get FIFO Status')
        return self.get_int('Memory FIFO status')
    
    def set_pid_address(self, address):
        assert 0 <= address <= 511, 'invalid address'
        self.set_int('PID_SetpointSelect', address)
        self.queue_register('Select PID Setpoint')
    
    def set_and_apply_pid_setpoints(self, address, setpoints, sweeps):
        assert len(setpoints) == 3, 'one setpoint is needed for each of the 3 channels'
        assert len(sweeps) == 3, 'one sweep point is needed for each of the 3 channels'
        assert 0 <= address <= 511, 'invalid address'
        self.set_int('PID_addr', address)

        gate_time = self.get_gate_time()

        for i, point in enumerate(setpoints):
            beat_div = self.get_beat_divider(i)
            beat_div2_enabled = self.get_beat_divider2_enabled(i)

            self.set_int('PID_S%d' % i, self.convert_to_count(
                point, gate_time, beat_div, beat_div2_enabled
            ))
        
        for i, sweep in enumerate(sweeps):
            self.set_bool('PID_W%d' % i, sweep)
        
        self.register_to_bus('Set PID Setpoint')
    
    def set_and_apply_pid_factors(self, channel, p, i):
        assert 0 <= channel <= 8, 'invalid channel'
        self.set_int('PID_Channel', channel)
        self.set_int('PID_I', i)
        self.set_int('PID_P', p)
        
        self.register_to_bus('Set PID Factors')
    
    def set_pid_signal_source(self, channel, source):
        """
        Sources: 0=counter0, 1=counter1, 2=counter2, 3=demod
        """
        assert 0 <= channel <= 8, 'invalid channel'
        assert 0 <= source <= 3, 'invalid source'

        self.set_int('PID%d_SignalSource' % channel, source)
        self.queue_register('Set PID signal source')
    
    def read_and_get_pid_int_overflow(self):
        self.register_to_bus('Get PID Overflow')
        return self.get_int('PID_IntOverflow')
    
    def set_and_apply_reset_pid_overflow(self):
        self.set_bool('PID_Reset_overflow', True)
        self.register_to_bus('Set PID Globals')