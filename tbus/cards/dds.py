from collections import namedtuple
from .card import TBusCard


DDS_MODES = namedtuple('DDS_MODES', ['manual', 'bus'])(False, True)
DDS_FREQ_MODES = namedtuple('DDS_FREQ_MODES', ['jump', 'sweep'])(False, True)
DDS_PHASE_MODES = namedtuple('DDS_PHASE_MODES', ['jump', 'sweep'])(False, True)
DDS_AMPL_MODES = namedtuple('DDS_PHASE_MODES', ['jump', 'sweep'])(False, True)


class DDSCard(TBusCard):
    def set_and_apply_channel(self, channel, address, frequency, amplitude, phase,
                              freq_mode=DDS_FREQ_MODES.jump,
                              ampl_mode=DDS_AMPL_MODES.jump,
                              phase_mode=DDS_PHASE_MODES.jump):
        """
        To which address should the settings be saved? (0 to 511)
        """
        assert channel in [0, 1], 'invalid channel'

        self.set_int('addr', address)
        self.set_int('freq', frequency)
        self.set_int('ampl', amplitude)
        self.set_int('phase', phase)
        self.set_bool('freqmode', freq_mode)
        self.set_bool('phasemode', phase_mode)
        self.set_bool('amplmode', ampl_mode)

        self.register_to_bus('SetMem_ch%d' % (channel + 1))

    def set_mode(self, mode):
        """
        Should the current address be set manually or by bus?
        `mode` has to be one of `DDS_MODES`.
        """
        self.set_bool('mode', mode)
        self.queue_register('SetSettings')

    def set_manual_address(self, address):
        """
        Which setting should be selected? `set_mode()` should be called first.
        """
        self.set_int('manualaddr', address)
        self.queue_register('SetSettings')

    def set_output_amplifier1_enabled(self, enabled):
        self.set_bool('enable1', enabled)
        self.queue_register('SetSettings')

    def set_output_amplifier2_enabled(self, enabled):
        self.set_bool('enable2', enabled)
        self.queue_register('SetSettings')

    def set_freq_sweep_rate_x256(self, enabled):
        self.set_bool('FX256', enabled)
        self.queue_register('SetSettings')

    def set_dds_clock_multiplier(self, value):
        self.set_int('DDS_factor', value)
        self.queue_register('Initialize')