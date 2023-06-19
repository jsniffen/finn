struct color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

int color_equals(struct color c1, struct color c2)
{
	return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

