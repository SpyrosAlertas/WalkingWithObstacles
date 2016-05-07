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


# ifndef __UTILITIES__
# define __UTILITIES__

# include <string>

// -------------------- Some useful functions --------------------
void Cube(double size);													// Draws a cube with texture coordinates
void Square(double size);												// Draws a square with texture coordinates
// -------------------- Some useful functions --------------------

// -------------------- Namespace: Utilities --------------------
namespace Utilities {
	void Init();														// Initialize Utilities Library
	void Update();														// Update Utilities Library
	void Exit();														// Clean up Utilities Library
};
// -------------------- Namespace: Utilities --------------------

// -------------------- Namespace: Audio --------------------
namespace Audio {
	void Load(std::string audio);										// Load audio file into memory
	void Unload(std::string audio);										// Unload audio file from memory

	void SetVolume(std::string audio, float volume);					// Change volume of specified sound
	void SetRepeat(std::string audio, bool repeat);						// Set if sound will play repeatedly
	void SetPaused(std::string audio, bool paused);						// Pause/Resume sound
	bool IsPlaying(std::string audio);									// Check if sound is currently playing

	void Play(std::string audio);										// Play sound - if audio file has beed loaded into memory
	void Stop(std::string audio);										// Stop playing audio

	void SetMasterVolume(float volume);									// Set master volume (All sounds volume will be multiplied with this value)
	float GetMasterVolume();											// Get value of master volume
	void PauseAll();													// Pause all sounds
	void ResumeAll();													// Resume all sounds
};
// -------------------- Namespace: Audio --------------------

// -------------------- Namespace: Texture --------------------
namespace Texture {
	void Enable();														// Enable Texturing (has to be called before using a texture - by default disabled)
	void Disable();														// Disable Texturing

	void Load(std::string texture);										// Load texture file into memory
	void Unload(std::string texture);									// Unload texture from memory

	void Use(std::string texture);										// Applies texture to the next displayed objects
};
// -------------------- Namespace: Texture --------------------

// -------------------- Namespace: Timer --------------------
namespace Timer {
	void Start();														// Start Timer
	void Stop();														// Stop Timer

	int GetElapsedTime();												// Get the elapsed time (int milliseconds)
};
// -------------------- Namespace: Timer --------------------

// -------------------- Namespace: Wavefront --------------------
namespace Wavefront {
	void Load(std::string wvf);											// Load wavefront object into memory
	void Unload(std::string wvf);										// Unload wavefront object from memory

	void Display(std::string wvf);										// Display wavefront object in the center of the screen
};
// -------------------- Namespace: Wavefront --------------------

# endif  // # ifndef __UTILITIES__