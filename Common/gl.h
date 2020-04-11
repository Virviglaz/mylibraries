#ifndef __GLH__
#define __GLH__

#define GL_POINT_MAX	1
#define GL_LINE_MAX	 1
#define GL_TRIANGLE_MAX 378

struct glVector3 {
	float x,y,z;
	uint16_t col;
};

void glLoadIdentity(void);

void glRotateXu(uint8_t alp);
void glRotateXf(float alp);
void glRotateYu(uint8_t alp);
void glRotateYf(float alp);
void glRotateZu(uint8_t alp);
void glRotateZf(float alp);
void glRotateu(uint8_t alp, float x, float y, float z);
void glRotatef(float alp, float x, float y, float z);

void glTranslatef(float x, float y, float z);

void glDrawPoint(struct glVector3 *p1);
void glDrawLine(struct glVector3 *_p1,
	struct glVector3 *_p2);
void glDrawTriangle(const struct glVector3 *t1,
	const struct glVector3 *t2,
		const struct glVector3 *t3);
void glDrawTriangleV(const struct glVector3 *t1,
	const struct glVector3 *t2,
		const struct glVector3 *t3,
			const struct glVector3 *n);
void glSwapBuffers(void (*draw_pic)(uint16_t x,
	uint16_t y, uint16_t dx, uint16_t dy, uint16_t *pic));
  
#endif /* __GLH__ */
