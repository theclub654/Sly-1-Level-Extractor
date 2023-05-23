#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

void decompress(unsigned char* input_data, uint32_t size, std::string levelname);

int main(int argc, char** argv)
{
	const char* filename = argv[1];

	if(filename == 0x0)
	{
		std::cout << "Drag Sly 1 Retail ISO file to program to extract level files NTSC Supported Only\n";
		return -1;
	}

	else
	{
		std::ifstream ISO(filename, std::ios::binary);
		static unsigned char* fileBuf;
		std::string name;
		uint32_t regionName;
		ISO.seekg(0x828BD, std::ios::beg);
		ISO.read(reinterpret_cast<char*>(&regionName), sizeof(uint32_t));
		std::string levelNames[45] = { "Paris", "The Hideout", "A Stealthy Approach", "Prowling the Grounds" ,"High Class Heist", "Into the Machine", "A Cunning Disguise"
		, "The Fire Down Below", "Treasure in the Depths", "The Gunboat Graveyard", "The Eye of the Storm", "A Rocky Start", "Muggshot's Turf", "Boneyard Casino", "Murray's Big Gamble"
		, "At the Dog Track", "Two to Tango", "Straight to the Top", "Back Alley Heist", "Last Call", "The Dread Swamp Path", "The Swamp's Dark Center", "The Lair of the Beast"
		, "A Grave Undertaking", "Piranha Lake", "Descent into Danger", "A Ghastly Voyage", "Down Home Cooking", "A Deadly Dance", "A Perilous Ascent", "Inside the Stronghold"
		, "Flaming Temple of Flame", "The Unseen Foe", "The King of the Hill", "Rapid Fire Assault", "Duel by the Dragon", "A Desperate Race", "Flame Fu!", "A Hazardous Path"
		, "Burning Rubber", "A Daring Rescue", "Bentley Comes Through", "A Temporary Truce", "Sinking Peril", "A Strange Reunion" };

		//NTSC
		if (regionName == 1398096723)
		{
			ISO.seekg(0x1D2B14, std::ios::beg);

			for (int i = 0; i < 45; i++)
			{
				ISO.seekg(0x8, SEEK_CUR);
				std::string name = levelNames[i];
				std::cout << "Extracting " << name << "\n";

				uint32_t temp0; // sector offset
				uint32_t temp1; // file size
				uint32_t temp2; // search value
				uint32_t temp3; // for lsn
				uint32_t temp4; // search cipher
				uint32_t temp5; // for size
				uint32_t temp6; // level ID
				uint32_t temp7; // Level Name

				ISO.read(reinterpret_cast<char*> (&temp0), sizeof(uint32_t));
				ISO.read(reinterpret_cast<char*> (&temp1), sizeof(uint32_t));
				ISO.read(reinterpret_cast<char*> (&temp2), sizeof(uint32_t));
				ISO.read(reinterpret_cast<char*> (&temp3), sizeof(uint32_t));
				ISO.read(reinterpret_cast<char*> (&temp4), sizeof(uint32_t));
				ISO.read(reinterpret_cast<char*> (&temp5), sizeof(uint32_t));
				ISO.read(reinterpret_cast<char*> (&temp6), sizeof(uint32_t));
				ISO.read(reinterpret_cast<char*> (&temp7), sizeof(uint32_t));
				uint32_t size = temp1 ^ temp7;
				long sectorOffset = temp0 ^ temp5;
				sectorOffset = sectorOffset * 0x800;
				fileBuf = new unsigned char[size];
				ISO.seekg(0x4, SEEK_CUR);
				long nextFileTable = ISO.tellg();
				ISO.seekg(sectorOffset, SEEK_SET);
				ISO.read((char*)fileBuf, size);
				decompress(fileBuf, size, name);
				ISO.seekg(nextFileTable, SEEK_SET);
			}
		}
		else
		{
			std::cout << "Invalid File\n";
			return -1;
		}

		std::cout << "Done\n";
		ISO.close();
	}
	return 0;
}

void decompress(unsigned char* input_data, uint32_t size, std::string levelname)
{
	//std::cout << std::hex << size;
	static char* output_data = new char[0x4000];
	static uint32_t actual_output_data_size = 10 * size;
	static char* actual_output_data = new char[actual_output_data_size];

	static uint64_t input_size = size;
	static uint64_t input_base = 0;
	static uint64_t output_base = 0;
	static uint64_t output_read = 0;

	static uint64_t input_pos = 0;
	static uint64_t output_pos = 0;

	static unsigned char bits = 0;
	static unsigned short src = 0;
	static short ssize = 0;
	static short offset = 0;
	static int i = 0;
	static unsigned long k = 0;

	while (input_pos < input_size) {
		bits = input_data[input_pos++];
		if (input_pos >= input_size)
			break;

		for (i = 0; i < 8; i++)
		{
			src = input_data[input_pos++];
			if (input_pos >= input_size)
				break;

			if (bits & 1) {
				output_data[output_pos++] = src;
				if (output_pos >= 0x2000)
				{
					output_pos &= 0x1fff;
					memcpy(actual_output_data + (k++ * 0x2000), output_data, 0x2000);
				}
			}
			else {
				src |= ((unsigned short)(input_data[input_pos++]) << 8);
				ssize = (src >> 13) + 2;
				offset = src & 0x1FFF;
				while (ssize >= 0) {
					--ssize;
					output_data[output_pos++] = output_data[offset];
					if (output_pos >= 0x2000)
					{
						output_pos &= 0x1fff;
						memcpy(actual_output_data + (k++ * 0x2000), output_data, 0x2000);
					}
					offset = (offset + 1) & 0x1FFF;
				}
			}
			bits >>= 1;
		}
	}

	if (output_pos >= 0)
		memcpy(actual_output_data + (k++ * 0x2000), output_data, output_pos);

	size_t gggg = k * 0x2000 + output_pos;

	std::ofstream output(levelname, std::ios::binary | std::ios::out);
	output.write(actual_output_data, gggg);
	output.close();
}
