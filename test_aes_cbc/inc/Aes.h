/*
 * rijndael-alg-fst.c
 *
 * @version 3.0 (December 2000)
 *
 * Optimised ANSI C code for the Rijndael cipher (now AES)
 *
 * @author Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>
 * @author Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>
 * @author Paulo Barreto <paulo.barreto@terra.com.br>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//#include <stdio.h>

#ifndef BYTE
	typedef	unsigned char			BYTE;
#endif
#ifndef UNIT
	typedef	unsigned int			UNIT;
#endif

void rijndaelKeySetupEnc(UNIT rk[/*44*/], const BYTE cipherKey[]);
void rijndaelKeySetupDec(UNIT rk[/*44*/], const BYTE cipherKey[]);
void rijndaelEncrypt(const UNIT rk[/*44*/], const BYTE pt[16], BYTE ct[16]);
void rijndaelDecrypt(const UNIT rk[/*44*/], const BYTE ct[16], BYTE pt[16]);
void aes_128_cbc_encrypt(const BYTE *key, const BYTE *iv, BYTE *data, size_t data_len);
void aes_128_cbc_decrypt(const BYTE *key, const BYTE *iv, BYTE *data, size_t data_len);
