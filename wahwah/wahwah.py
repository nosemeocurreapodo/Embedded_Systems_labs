import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import wave


def wah_wah_filter(input_signal, sample_rate, min_freq=500, max_freq=3000, lfo_freq=0.7, Q=1):
    """
    Wah-Wah filter implementation.

    Parameters:
        input_signal (numpy array): Input audio signal.
        sample_rate (int): Sampling rate of the signal in Hz.
        min_freq (float): Minimum cutoff frequency of the Wah effect (Hz).
        max_freq (float): Maximum cutoff frequency of the Wah effect (Hz).
        lfo_freq (float): Frequency of the LFO (Hz).
        Q (float): Quality factor of the bandpass filter.

    Returns:
        numpy array: Filtered output signal.
    """
    # Time vector for the input signal
    t = np.arange(len(input_signal)) / sample_rate

    # Generate the LFO (Low-Frequency Oscillator) for modulating the center frequency
    lfo = 0.5 * (1 + np.sin(2 * np.pi * lfo_freq * t))  # Normalized between 0 and 1
    center_freq = min_freq + lfo * (max_freq - min_freq)

    # Initialize filter states
    output_signal = np.zeros_like(input_signal)
    x1, x2, y1, y2 = 0, 0, 0, 0  # Delays for the filter

    for n, sample in enumerate(input_signal):
        # Compute angular frequency and coefficients for the current center frequency
        omega_c = 2 * np.pi * center_freq[n] / sample_rate
        alpha = np.sin(omega_c) / (2 * Q)
        
        b0 = alpha
        b1 = 0
        b2 = -alpha
        a0 = 1 + alpha
        a1 = -2 * np.cos(omega_c)
        a2 = 1 - alpha
        
        # Normalize coefficients
        b0 /= a0
        b1 /= a0
        b2 /= a0
        a1 /= a0
        a2 /= a0

        # Apply the difference equation
        y = b0 * sample + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2

        # Update delays
        x2 = x1
        x1 = sample
        y2 = y1
        y1 = y

        # Save the output
        output_signal[n] = y

    return output_signal

# Example Usage
if __name__ == "__main__":
    # Generate a sample input signal (sine wave)
    sample_rate = 48000  # 48 kHz
    duration = 5  # 5 seconds
    t = np.linspace(0, duration, int(sample_rate * duration), endpoint=False)
    
    #use pure sin as input signal
    #input_signal = 0.5 * np.sin(2 * np.pi * 440 * t)  # A440 tone (440 Hz)

    #use a .wav file as input signal
    wav_path = "recording_0.wav"
    with wave.open(wav_path, 'r') as wav_file:
        raw_frames = wav_file.readframes(-1)
        num_frames = wav_file.getnframes()
        num_channels = wav_file.getnchannels()
        sample_rate = wav_file.getframerate()
        sample_width = wav_file.getsampwidth()
            
    temp_buffer = np.empty((num_frames, num_channels, 4), dtype=np.uint8)
    raw_bytes = np.frombuffer(raw_frames, dtype=np.uint8)
    temp_buffer[:, :, :sample_width] = raw_bytes.reshape(-1, num_channels, 
                                                        sample_width)
    temp_buffer[:, :, sample_width:] = \
        (temp_buffer[:, :, sample_width-1:sample_width] >> 7) * 255
    frames = temp_buffer.view('<i4').reshape(temp_buffer.shape[:-1])

    input_signal = frames[:, 0]

    # Apply the Wah-Wah filter
    filtered_signal = wah_wah_filter(input_signal, sample_rate)

    # Plot the original and filtered signals
    plt.figure(figsize=(10, 6))
    plt.plot(t, input_signal, label="Original Signal", alpha=0.7)
    plt.plot(t, filtered_signal, label="Wah-Wah Filtered Signal", alpha=0.7)
    plt.xlabel("Time (s)")
    plt.ylabel("Amplitude")
    plt.title("Wah-Wah Filter Effect")
    plt.legend()
    plt.show()
