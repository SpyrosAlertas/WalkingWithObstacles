/* MIT License
*
* Copyright(c) 2016 Spyros Alertas
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* (copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/


# include <unordered_map>
# include <iostream>
# include <queue>
# include <fstream>

# include "gl\glut.h"
# include "fmod\fmod.hpp"
# include "corona\corona.h"

// -------------------- Static functions - to be used only in this file -------------------
static void Planar_Mapping(float size, float x, float y, float z, float v1[3], float v2[3]) {
	float u, t;
	u = x*v1[0] + y*v1[1] + z*v1[2];
	t = x*v2[0] + y*v2[1] + z*v2[2];
	glTexCoord2f(u / size, t / size);
}

static void Mapping_Function(float x, float y, float z, int plane, double size) {	// Chooses the level (xy, yz, zx) where planar mapping will occur
	float vx[3], vy[3], vz[3];
	vx[0] = 1; vx[1] = 0; vx[2] = 0;
	vy[0] = 0; vy[1] = 1; vy[2] = 0;
	vz[0] = 0; vz[1] = 0; vz[2] = 1;
	switch (plane) {
	case 0: Planar_Mapping(size, x, y, z, vx, vy); break;
	case 1: Planar_Mapping(size, x, y, z, vy, vz); break;
	case 2: Planar_Mapping(size, x, y, z, vx, vz); break;
	}
	glVertex3f(x, y, z);
}
// -------------------- Static functions - to be used only in this file --------------------

// -------------------- Some useful functions --------------------
void Cube(GLdouble size) {												// Draws a cube with texture coordinates
	size /= 2;
	glBegin(GL_QUADS);
	// Draw Right side of cube
	glNormal3f(1.0, 0.0, 0.0);
	Mapping_Function(size, size, size, 1, size);
	Mapping_Function(size, -size, size, 1, size);
	Mapping_Function(size, -size, -size, 1, size);
	Mapping_Function(size, size, -size, 1, size);
	// Draw Left side of cube
	glNormal3f(-1.0, 0.0, 0.0);
	Mapping_Function(-size, size, -size, 1, size);
	Mapping_Function(-size, -size, -size, 1, size);
	Mapping_Function(-size, -size, size, 1, size);
	Mapping_Function(-size, size, size, 1, size);
	// Draw Up side of cube
	glNormal3f(0.0, 1.0, 0.0);
	Mapping_Function(-size, size, -size, 2, size);
	Mapping_Function(-size, size, size, 2, size);
	Mapping_Function(size, size, size, 2, size);
	Mapping_Function(size, size, -size, 2, size);
	// Draw Down side of cube
	glNormal3f(0.0, -1.0, 0.0);
	Mapping_Function(-size, -size, size, 2, size);
	Mapping_Function(-size, -size, -size, 2, size);
	Mapping_Function(size, -size, -size, 2, size);
	Mapping_Function(size, -size, size, 2, size);
	// Draw Front side of cube
	glNormal3f(0.0, 0.0, -1.0);
	Mapping_Function(-size, size, -size, 0, size);
	Mapping_Function(size, size, -size, 0, size);
	Mapping_Function(size, -size, -size, 0, size);
	Mapping_Function(-size, -size, -size, 0, size);
	// Draw Back side of cube
	glNormal3f(0.0, 0.0, 1.0);
	Mapping_Function(size, -size, size, 0, size);
	Mapping_Function(size, size, size, 0, size);
	Mapping_Function(-size, size, size, 0, size);
	Mapping_Function(-size, -size, size, 0, size);
	glEnd();
}

void Square(GLdouble size) {											// Draws a square with texture coordinates
	size /= 2.0;
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-size, -size, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-size, size, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(size, size, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(size, -size, 0.0);
	glEnd();
}
// -------------------- Some useful functions --------------------

// -------------------- Namespace: Audio --------------------
namespace Audio {
# define MAX_CHANNELS 100

	static FMOD::System* system;
	static float master_volume = 100.0;

	class Sound {
	public:
		FMOD::Sound* sound;
		FMOD::Channel* channel;
		double volume;
	};

	static bool freeChannels[MAX_CHANNELS];

	static std::unordered_map <std::string, Sound> sounds;				// Hash map with all loaded sounds

	static void Init() {												// Initialize Audio Library
		unsigned int version;
		int numdrivers;
		char name[256];
		FMOD_CAPS caps;
		FMOD_SPEAKERMODE speakermode;
		FMOD_RESULT error;
		FMOD::System_Create(&system);
		system->getVersion(&version);
		if (version < FMOD_VERSION) {
			std::cerr << "Error! You are using an old version of FMOD " << version << ". This program requires " << FMOD_VERSION << " or higher" << std::endl;
			return;
		}
		system->getNumDrivers(&numdrivers);
		if (numdrivers == 0) {
			system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		}
		else {
			system->getDriverCaps(0, &caps, 0, &speakermode);
			system->setSpeakerMode(speakermode);
			if (caps & FMOD_CAPS_HARDWARE_EMULATED)
				system->setDSPBufferSize(1024, 10);
		}
		system->getDriverInfo(0, name, 256, 0);
		if (strstr(name, "SigmaTel"))
			system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
		error = system->init(MAX_CHANNELS, FMOD_INIT_NORMAL, 0);
		if (error == FMOD_ERR_OUTPUT_CREATEBUFFER) {					// Ok, the speaker mode selected isn't supported by this soundcard. Switch it back to stereo...
			system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
			system->init(MAX_CHANNELS, FMOD_INIT_NORMAL, 0);			// ... and re-init.
		}
		for (int i = 0; i < MAX_CHANNELS; i++)
			freeChannels[i] = true;
		std::cout << "Initialized Audio Library.." << std::endl;
	}

	static void Update() {												// Update Audio Library
		system->update();
	}

	static void Exit() {												// Clean up Audio Library
		std::unordered_map <std::string, Sound>::iterator it;
		for (it = sounds.begin(); it != sounds.end(); it++)
			it->second.sound->release();
		sounds.clear();
		system->close();
		system->release();
		std::cout << "Cleaned Audio Library.." << std::endl;
	}

	void Load(std::string _audio) {										// Load audio file into memory
		if (sounds.find(_audio) != sounds.end())						// If sound already exists in hash map
			return;														// Don't delay reading it from file
		std::string audio = "sounds\\" + _audio + ".ogg";
		Sound sound;
		int i;
		for (i = 0; i < MAX_CHANNELS - 1; i++)
			if (freeChannels[i] == true) {
				freeChannels[i] = false;
				break;
			}
		sound.volume = 1.0;
		if (system->createSound(audio.c_str(), FMOD_HARDWARE, 0, &(sound.sound)) != FMOD_OK)
			std::cout << "Missing audio: " << _audio << ".ogg" << std::endl;
		else {
			std::cout << "Loaded audio: " << _audio << ".ogg" << std::endl;
			system->getChannel(i, &sound.channel);
			sounds[_audio] = sound;										// Insert new sound in hash map if it doesn't exist already
		}
	}

	void Unload(std::string audio) {									// Unload audio file from memory
		std::unordered_map <std::string, Sound>::iterator it = sounds.find(audio);	// Find sound in hash map
		if (it != sounds.end()) {										// If sound exists in hash map
			it->second.sound->release();
			sounds.erase(audio);										// Erase sound from hash map
			std::cout << "Unloaded audio: " << audio << ".ogg" << std::endl;
		}
	}

	void SetVolume(std::string audio, float volume) {					// Change volume of specified sound
		std::unordered_map <std::string, Sound>::iterator it = sounds.find(audio);	// Find sound in hash map
		if (it != sounds.end()) {										// If sound exists in hash map
			it->second.volume = volume;
			it->second.channel->setVolume((it->second.volume*(master_volume / 100.0)));
		}
	}

	void SetRepeat(std::string audio, bool repeat) {
		std::unordered_map <std::string, Sound>::iterator it = sounds.find(audio);	// Find sound in hash map
		if (it != sounds.end())											// If sound exists in hash map
			if (repeat == true)
				it->second.sound->setMode(FMOD_LOOP_NORMAL);
			else
				it->second.sound->setMode(FMOD_LOOP_OFF);
	}

	void SetPaused(std::string audio, bool paused) {
		std::unordered_map <std::string, Sound>::iterator it = sounds.find(audio);	// Find sound in hash map
		if (it != sounds.end())											// If sound exists in hash map
			it->second.channel->setPaused(paused);
	}

	bool IsPlaying(std::string audio) {
		std::unordered_map <std::string, Sound>::iterator it = sounds.find(audio);	// Find sound in hash map
		bool isPlaying = false;
		if (it != sounds.end())											// If sound exists in hash map
			it->second.channel->isPlaying(&isPlaying);
		return isPlaying;
	}

	void Play(std::string audio) {										// Play audio file - if file has beed loaded into memory
		std::unordered_map <std::string, Sound>::iterator it = sounds.find(audio);	// Find sound in hash map
		if (it != sounds.end()) {										// If sound exists in hash map
			system->playSound(FMOD_CHANNEL_FREE, it->second.sound, false, &it->second.channel);	// Play the audio
			it->second.channel->setVolume((it->second.volume*(master_volume / 100.0)));
		}
	}

	void Stop(std::string audio) {
		std::unordered_map <std::string, Sound>::iterator it = sounds.find(audio);	// Find sound in hash map
		if (it != sounds.end())											// If sound exists in hash map
			it->second.channel->stop();
	}

	void SetMasterVolume(float volume) {								// Set master volume (All sounds volume will be multiplied with this value)
		if (volume < 0)
			volume = 0;
		else if (volume > 100)
			volume = 100;
		master_volume = volume;
		std::unordered_map <std::string, Sound>::iterator it;
		for (it = sounds.begin(); it != sounds.end(); it++) {
			it->second.channel->setVolume((it->second.volume*(master_volume / 100.0)));
		}
	}

	float GetMasterVolume() {											// Get value of master volume
		return master_volume;
	}

	void PauseAll() {													// Pause all sounds
		std::unordered_map <std::string, Sound>::iterator it;
		for (it = sounds.begin(); it != sounds.end(); it++)
			it->second.channel->setPaused(true);
	}

	void ResumeAll() {													// Resume all sounds
		std::unordered_map <std::string, Sound>::iterator it;
		for (it = sounds.begin(); it != sounds.end(); it++)
			it->second.channel->setPaused(false);
	}
};
// -------------------- Namespace: Audio --------------------

// -------------------- Namespace: Texture --------------------
namespace Texture {
	static std::unordered_map <std::string, GLuint> textures;			// Hash map with all loaded textures

	static void Init() {												// Initialize Texture Library
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		std::cout << "Initialized Texture Library.." << std::endl;
	}

	static void Exit() {												// Clean up Texture Library
		std::unordered_map <std::string, GLuint>::iterator it;
		for (it = textures.begin(); it != textures.end(); it++)
			glDeleteTextures(1, &it->second);
		textures.clear();
		std::cout << "Cleaned Texture Library.." << std::endl;
	}

	void Enable() {														// Enable Texturing (has to be called before using a texture - by default disabled)
		glEnable(GL_TEXTURE_2D);
	}

	void Disable() {													// Disable Texturing
		glDisable(GL_TEXTURE_2D);
	}

	void Load(std::string _texture) {									// Load texture file into memory
		if (textures.find(_texture) != textures.end())					// If texture already exists in hash map
			return;
		std::string texture = "textures\\" + _texture + ".jpg";
		GLuint newTexture;
		corona::Image* image = corona::OpenImage(texture.c_str(), corona::PF_B8G8R8);
		if (image == NULL) {
			std::cerr << "Missing texture: " << _texture << ".jpg" << std::endl;
			return;
		}
		glGenTextures(1, &newTexture);
		glBindTexture(GL_TEXTURE_2D, newTexture);
		// Define what happens if given (s,t) outside [0,1] {REPEAT, CLAMP}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image->getWidth(), image->getHeight(), GL_BGR_EXT, GL_UNSIGNED_BYTE, image->getPixels());
		textures[_texture] = newTexture;								// Insert new texture in hash map if it doesn't exist already
		std::cout << "Loaded texture: " << _texture << ".jpg" << std::endl;
	}

	void Unload(std::string texture) {									// Unload texture from memory
		std::unordered_map <std::string, GLuint>::iterator it = textures.find(texture);	// Find texture in hash map
		if (it != textures.end()) {										// If texture exists in hash map
			glDeleteTextures(1, &it->second);
			textures.erase(texture);
			std::cout << "Unloaded texture: " << texture << ".jpg" << std::endl;
		}
	}

	void Use(std::string texture) {										// Applies texture to the next displayed objects
		std::unordered_map <std::string, GLuint>::iterator it = textures.find(texture);	// Find texture in hash map
		if (it != textures.end())										// If texture exists in hash map
			glBindTexture(GL_TEXTURE_2D, it->second);
	}
};
// -------------------- Namespace: Texture --------------------

// -------------------- Namespace: Timer --------------------
namespace Timer {
	static int startingTime = 0;
	static int elapsedTime = 0;

	static void Init() {												// Initialize Timer for use
		std::cout << "Initialized Timer Library.." << std::endl;
	}

	static void Exit() {												// Clean up Timer Library
		std::cout << "Cleaned Timer Library.." << std::endl;
	}

	void Start() {														// Start Timer
		startingTime = glutGet(GLUT_ELAPSED_TIME);
	}

	void Stop() {														// Stop Timer
		elapsedTime = glutGet(GLUT_ELAPSED_TIME) - startingTime;
	}

	int GetElapsedTime() {												// Get the elapsed time (in milliseconds)
		return elapsedTime;
	}
};
// --------------------Namespace: Timer --------------------

// -------------------- Namespace: Wavefront --------------------
namespace Wavefront {
	typedef struct Point3D {
		GLfloat x, y, z, w;
	} Point3D;

	typedef struct Object {
		Point3D *v, *vn;												// Array of the objects geometric vertices & vertex normals
		int *vf, *vnf;													// Array of objects faces for geometric vertices & vertex normals
		int nfaces;														// Number of faces
	} Object;

	static std::unordered_map <std::string, Object> objects;			// Hash map with all loaded wavefront objects

	static void Init() {												// Initialize Wavefront Library
		objects.clear();
		std::cout << "Initialized Wavefront Library.." << std::endl;
	}

	static void Exit() {												// Clean up Wavefront Library
		std::unordered_map <std::string, Object>::iterator it;
		for (it = objects.begin(); it != objects.end(); it++) {
			delete it->second.v;
			delete it->second.vn;
			delete it->second.vnf;
			delete it->second.vf;
		}
		objects.clear();
		std::cout << "Cleaned Wavefront Library.." << std::endl;
	}

	void Load(std::string _wvf) {										// Load wavefront object into memory
		if (objects.find(_wvf) != objects.end())						// If wavefront object already exists in hash map
			return;														// Don't delay reading it from file
		std::string wvf = "models\\" + _wvf + ".obj";
		std::ifstream file(wvf);
		if (file.is_open()) {											// If file opened succesfully
			std::queue <Point3D> vertices, normals;
			std::queue <int> vf, nf;
			Point3D tmp;
			Object object;
			char ch;
			std::string str;
			int a, b;
			while (file >> str) {
				if (str == "#")
					getline(file, str);
				else if (str == "g")
					getline(file, str);
				else if (str == "v") {
					file >> tmp.x;
					file >> tmp.y;
					file >> tmp.z;
					file >> ch;
					if (ch >= '0' && ch <= '9') {
						file.putback(ch);
						file >> tmp.w;
					}
					else {
						file.putback(ch);
						tmp.w = 1.000000;
					}
					vertices.push(tmp);
				}
				else if (str == "vn") {
					file >> tmp.x;
					file >> tmp.y;
					file >> tmp.z;
					normals.push(tmp);
				}
				else if (str == "f") {
					file >> a;
					file >> ch;
					file >> ch;
					file >> b;
					vf.push(a);
					nf.push(b);
					file >> a;
					file >> ch;
					file >> ch;
					file >> b;
					vf.push(a);
					nf.push(b);
					file >> a;
					file >> ch;
					file >> ch;
					file >> b;
					vf.push(a);
					nf.push(b);
				}
			}
			int vertices_number = object.nfaces = vertices.size();
			object.v = new Point3D[object.nfaces];
			for (a = 0; a < object.nfaces; a++) {
				object.v[a] = vertices.front();
				vertices.pop();
			}
			int normals_number = object.nfaces = normals.size();
			object.vn = new Point3D[object.nfaces];
			for (a = 0; a < object.nfaces; a++) {
				object.vn[a] = normals.front();
				normals.pop();
			}
			int faces_number = object.nfaces = nf.size();
			object.vf = new int[object.nfaces];
			object.vnf = new int[object.nfaces];
			for (a = 0; a < object.nfaces; a++) {
				object.vf[a] = vf.front();
				vf.pop();
				object.vnf[a] = nf.front();
				nf.pop();
			}
			objects[_wvf] = object;										// Insert new wavefront object in hash map if it doesn't exist already
			std::cout << "Loaded wavefront: " << _wvf << ".obj" << " (Vertices # = " << vertices_number
				<< ", Normals # = " << normals_number << ", Faces # = " << faces_number / 3.0 << std::endl;
		}
		else
			std::cerr << "Missing wavefront: " << _wvf << ".obj" << std::endl;
	}

	void Unload(std::string wvf) {										// Unload wavefront object from memory
		std::unordered_map <std::string, Object>::iterator it = objects.find(wvf);	// Find wavefront in hash map
		if (it != objects.end()) {										// If wavefront exists in hash map
			delete it->second.v;
			delete it->second.vn;
			delete it->second.vnf;
			delete it->second.vf;
			objects.erase(wvf);
			std::cout << "Unloaded wavefront: " << wvf << ".obj" << std::endl;
		}
	}

	void Display(std::string wvf) {										// Display wavefront object in the center of the screen
		std::unordered_map <std::string, Object>::iterator it = objects.find(wvf);	// Find wavefront in hash map
		if (it != objects.end()) {										// If wavefront exists in hash map
			glBegin(GL_TRIANGLES);
			for (int i = 0; i < it->second.nfaces - 20; i += 3) {		// For all the faces that were read, use 3 each time to draw a triangle
				glNormal3f(it->second.vn[it->second.vnf[i] - 1].x, it->second.vn[it->second.vnf[i] - 1].y, it->second.vn[it->second.vnf[i] - 1].z);	// Use normals loaded from file
				glVertex4f(it->second.v[it->second.vf[i] - 1].x, it->second.v[it->second.vf[i] - 1].y, it->second.v[it->second.vf[i] - 1].z, it->second.v[it->second.vf[i]].w);	// Use 1st point for triangle
				glNormal3f(it->second.vn[it->second.vnf[i + 1] - 1].x, it->second.vn[it->second.vnf[i + 1] - 1].y, it->second.vn[it->second.vnf[i + 1] - 1].z);	// Use normals loaded from file
				glVertex4f(it->second.v[it->second.vf[i + 1] - 1].x, it->second.v[it->second.vf[i + 1] - 1].y, it->second.v[it->second.vf[i + 1] - 1].z, it->second.v[it->second.vf[i + 1]].w);	// Use 2nd point for triangle
				glNormal3f(it->second.vn[it->second.vnf[i + 2] - 1].x, it->second.vn[it->second.vnf[i + 2] - 1].y, it->second.vn[it->second.vnf[i + 2] - 1].z);	// Use normals loaded from file
				glVertex4f(it->second.v[it->second.vf[i + 2] - 1].x, it->second.v[it->second.vf[i + 2] - 1].y, it->second.v[it->second.vf[i + 2] - 1].z, it->second.v[it->second.vf[i + 2]].w);	// Use 3d & last point for triangle
			}
			glEnd();
		}
	}
};
// -------------------- Namespace: Wavefront --------------------

// -------------------- Namespace: Utilities --------------------
namespace Utilities {
	void Init() {														// Initialize Utilities Library
		Audio::Init();													// Initialize Audio Library
		Texture::Init();												// Initiaize Texture Library
		Timer::Init();													// Initialize Timer Library
		Wavefront::Init();												// Initialize Wavefront Library
		std::cout << "Initialized Utilities Library..\n" << std::endl;
	}

	void Update() {
		Audio::Update();												// Update Audio Library
	}

	void Exit() {														// Clean up Utilities Library
		Audio::Exit();													// Clean up Audio Library
		Texture::Exit();												// Clean up Texture Library
		Timer::Exit();													// Clean up Timer Library
		Wavefront::Exit();												// Clean up Wavefront Library
		std::cout << "Cleaned Utilities Library.." << std::endl;
	}
};
// -------------------- Namespace: Utilities --------------------