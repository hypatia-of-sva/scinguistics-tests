#include "common.h"

/*
	Reads a 16 bit PCM Wave file bytewise; kills the program if it doesn't work (gory!)
	For documentation see the specs and overview at
	https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
*/
waveform_t read_amplitude_data(char* file_name, int chosen_channel) {
	FILE* fp;
	size_t remaining, block_remaining;
	char block_label[4];
	int found_format_block = 0;
	int available_channels, bytes_per_sample;
    int i, ch;
	waveform_t wave;

	fp = fopen(file_name, "r");
	if(fgetc(fp) != 'R' || fgetc(fp) != 'I' || fgetc(fp) != 'F' || fgetc(fp) != 'F')
		die("invalid file (invalid \"RIFF\" tag)");
	remaining = fgetc(fp)+(fgetc(fp)<<8)+(fgetc(fp)<<16)+(fgetc(fp)<<24);
	if(filesize(fp) < remaining+8)
		die("invalid file (file shorter than described in the header)");
	if(fgetc(fp) != 'W' || fgetc(fp) != 'A' || fgetc(fp) != 'V' || fgetc(fp) != 'E')
		die("invalid file (invalid \"WAVE\" tag)");
	remaining -= 4;

	while(remaining > 0) {
		block_label[0] = fgetc(fp);
		block_label[1] = fgetc(fp);
		block_label[2] = fgetc(fp);
		block_label[3] = fgetc(fp);
		block_remaining = fgetc(fp)+(fgetc(fp)<<8)+(fgetc(fp)<<16)+(fgetc(fp)<<24);
		if(remaining < block_remaining+8)
			die("invalid file (block within the file longer than the described file size)");
		remaining -= 8 + block_remaining + (block_remaining%2);
		switch(block_label[0]) {
			case 'f':
				if(block_label[1] != 'm' || block_label[2] != 't' || block_label[3] != ' ') {
					while(block_remaining > 0) {
						fgetc(fp);
						block_remaining--;
					}
					break; /* the whole case */
				}

				if(block_remaining != 16)
					die("invalid format (format block size greater than expected for PCM)");
				if(fgetc(fp) != 1 || fgetc(fp) != 0)
					die("invalid format (format tag different from 0x0001 (PCM))");

				available_channels = fgetc(fp) + (fgetc(fp)<<8);
				if(chosen_channel > available_channels)
					die("invalid format (not enough channels available)");

				wave.samples_per_second = fgetc(fp)+(fgetc(fp)<<8)+(fgetc(fp)<<16)+(fgetc(fp)<<24);

				(void) (fgetc(fp)+(fgetc(fp)<<8)+(fgetc(fp)<<16)+(fgetc(fp)<<24));

				bytes_per_sample = (fgetc(fp)+(fgetc(fp)<<8))/available_channels;
				if (bytes_per_sample != 2)
					die("invalid format (bits per sample not 16)");

				(void) (fgetc(fp)+(fgetc(fp)<<8));

				found_format_block = 1;
			break;
			case 'd':
				if(block_label[1] != 'a' || block_label[2] != 't' || block_label[3] != 'a') {
					while(block_remaining > 0) {
						fgetc(fp);
						block_remaining--;
					}
					break; /* the whole case */
				}

				if(!found_format_block)
					die("invalid file (no format block found before data block)");

				wave.data_length = block_remaining/(2*available_channels);
				wave.amplitude_data = malloc(sizeof(int16_t)*wave.data_length);
				for(i = 0; i < wave.data_length; i++) {
					for(ch = 0; ch < available_channels; ch++) {
						if (ch == chosen_channel) {
							wave.amplitude_data[i] = (fgetc(fp)+(fgetc(fp)<<8));
						} else {
							(void) (fgetc(fp)+(fgetc(fp)<<8));
						}
					}
				}
				if((block_remaining%2) == 1) (void) (fgetc(fp));
			break;


			default:
				while(block_remaining > 0) {
					fgetc(fp);
					block_remaining--;
				}
			break;
		}
	}

	fclose(fp);
	return wave;
}
