alphahide 🕵️‍♂️

Hide data inside PNG images using the alpha channel (LSB steganography).

Because normal files are boring.

---

✨ Features

- Encode any file into a PNG image
- Decode hidden data back to original
- Uses alpha channel (minimal visual impact)
- Works with stdin / stdout (UNIX-friendly)
- No external dependencies (just lodepng)

---

⚙️ Build

g++ -O2 alphahide.cpp lodepng.cpp -o alphahide

---

🚀 Usage

Encode (file input)

./alphahide encode input.png output.png secret.txt

---

Encode (stdin)

cat secret.txt | ./alphahide encode input.png output.png

---

Decode

./alphahide decode output.png > recovered.txt

---

📦 How it works

- Each pixel has RGBA (Red, Green, Blue, Alpha)
- This tool stores 1 bit per pixel in the alpha channel
- Data is converted to bits and embedded sequentially
- A delimiter marks the end of the hidden data

---

📏 Capacity

max_data ≈ (image_width × image_height) / 8 bytes

Example:

- 1024×1024 image → ~131 KB max

---

⚠️ Important Notes

- Use PNG only
- Do NOT convert to JPEG (data will be destroyed)
- Avoid apps that:
  - compress images
  - strip alpha channel
- Always test decoding after encoding

---

🔐 Tips

For extra security, encrypt your data before embedding:

gpg -c secret.txt | ./alphahide encode input.png output.png

Decode:

./alphahide decode output.png | gpg -d

---

🧪 Limitations

- 1 bit per pixel (low capacity)
- No error correction (corruption = broken data)
- Requires alpha channel preservation

---

🧠 Future Ideas

- [ ] Multi-bit encoding (2–4 bits per pixel)
- [ ] Password-based pixel shuffling
- [ ] Header with size + checksum
- [ ] Error correction (Reed-Solomon)
- [ ] TUI interface

---

⚖️ Disclaimer

For educational and experimental use only.

If you're hiding something, you probably already know why.

---

💤 Author Notes

Written late at night with questionable sleep decisions.
If something breaks, blame past me.
