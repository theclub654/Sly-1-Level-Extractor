#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

int main(int argc, char** argv)
{
	const char* filename = argv[1];

	if(filename == 0x0)
	{
		std::cout << "Drag Sly 1 Retail ISO file to program to extract level files\n";
		return -1;
	}

	else
	{
		std::ifstream ISO(filename, std::ios::binary);
		std::streampos size;
		std::string name;
		static unsigned char* fileBuf;
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
				std::ofstream output(name, std::ios::binary | std::ios::out);
				std::cout << "Extracting " << name << "\n";
				uint32_t temp0;
				uint32_t temp1;
				uint32_t temp2;
				uint32_t temp3;
				uint32_t temp4;
				uint32_t temp5;
				uint32_t temp6;
				uint32_t temp7;
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
				output.write((char*)fileBuf, size);
				output.close();
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
