#ifndef __GXAV_BITOPS_H__
#define __GXAV_BITOPS_H__

//reg
#define gx_ioread32(addr)          (*(volatile int *)(addr))
#define gx_iowrite32(value, addr)  (*(volatile int *)(addr)=(value))
#define gx_clear_bit(nr, addr)     (*(volatile unsigned int *)addr) &= ~(1<<(nr))
#define gx_set_bit(nr, addr)       (*(volatile unsigned int *)addr) |= 1<<(nr)
#define gx_test_bit(nr, addr)      test_bit(nr,(const volatile long unsigned int *)addr)

#define REG_SET_BIT64(reg, bit) do {            \
	gx_set_bit(reg[bit >> 5], bit % 32);    \
}while(0)

#define REG_CLR_BIT64(reg, bit) do {            \
	gx_clear_bit(reg[bit >> 5], bit % 32);  \
}while(0)

#define REG_SET_BITS(reg,bits) do {             \
	unsigned int Reg = gx_ioread32(reg);    \
	Reg   |=  (bits);                       \
	gx_iowrite32(Reg, reg);                 \
}while(0)

#define REG_CLR_BITS(reg,bits) do {             \
	unsigned int Reg = gx_ioread32(reg);    \
	Reg   &=  ~(bits);                      \
	gx_iowrite32(Reg, reg);                 \
}while(0)

#define REG_GET_FIELD(reg,mask,offset)          \
	((gx_ioread32(reg) & (mask)) >> (offset))

#define REG_SET_VAL(reg, val)                   \
	gx_iowrite32(val, reg)

#define REG_GET_BYTE0(reg)  ((reg)  &  0xFF)
#define REG_GET_BYTE1(reg)  (((reg) >> 8) & 0xFF)
#define REG_GET_BYTE2(reg)  (((reg) >> 16) & 0xFF)
#define REG_GET_BYTE3(reg)  (((reg) >> 24) & 0xFF)

#if (defined LITTLE_ENDIAN)||(defined __KERNEL__)

#define CHANGE_ENDIAN_32(reg) do {              \
	unsigned int Reg = (reg);               \
	Reg  = ((REG_GET_BYTE0(Reg) << 24) |    \
	(REG_GET_BYTE1(Reg) << 16) |            \
	(REG_GET_BYTE2(Reg) << 8 ) |            \
	(REG_GET_BYTE3(Reg))) ;                 \
	(reg)   =   (Reg) ;                     \
}while(0)

#define GET_ENDIAN_32(reg)                      \
    ((REG_GET_BYTE0(reg) << 24) |               \
     (REG_GET_BYTE1(reg) << 16) |               \
     (REG_GET_BYTE2(reg) << 8 ) |               \
     (REG_GET_BYTE3(reg)))

#define GET_ENDIAN_16(reg)                      \
     (REG_GET_BYTE0(reg) << 8 ) |               \
     (REG_GET_BYTE1(reg))

#else
#define CHANGE_ENDIAN_32(reg)
#define GET_ENDIAN_32(reg)     (reg)
#define GET_ENDIAN_16(reg)     (reg)
#endif


#define GX_SET_BIT(reg,bit) do{                \
	gx_set_bit(bit, reg);                  \
}while(0)

#define GX_CLR_BIT(reg,bit) do {               \
	gx_clear_bit(bit, reg);                \
}while(0)

#define GX_GET_BIT(reg,bit) do {               \
	gx_test_bit(bit, reg);                 \
}while(0)

#define GX_SET_FEILD(reg,mask,val,offset)  do{ \
	unsigned int Reg = gx_ioread32(reg);   \
	Reg  &=  ~(mask);                      \
	Reg  |=  ((val) << (offset)) & (mask); \
	gx_iowrite32(Reg, reg);                \
}while(0)

#define GX_GET_FEILD(reg,mask,val, offset)\
	(val) = ((gx_ioread32(reg)&(mask))>>(offset))

#define GX_SET_VAL(reg,val)                    \
    gx_iowrite32(val, reg)

#define GX_SET_VAL_E(reg,val) do {             \
	unsigned int tmpVal = val;             \
	CHANGE_ENDIAN_32(tmpVal) ;             \
	gx_iowrite32(tmpVal, reg);             \
}while(0)

#define GX_GET_VAL_E(reg,val) do {             \
	val = gx_ioread32(reg);                \
	val = CHANGE_ENDIAN_32(val) ;          \
}while(0)

#define GX_CMD_SET_VAL_E(reg,val) do {         \
    unsigned int tmpVal = val;                 \
	CHANGE_ENDIAN_32(tmpVal) ;             \
	gx_iowrite32(tmpVal, reg);             \
}while(0)

#define GX_CMD_SET_VAL(reg,val) do {           \
    unsigned int tmpVal = val;                 \
	gx_iowrite32(tmpVal, reg);             \
}while(0)

#define GX_SET_FEILD_E(reg,mask,val,offset) do { \
	unsigned int tmpVal  = gx_ioread32(reg); \
	CHANGE_ENDIAN_32(tmpVal) ;               \
	tmpVal  &=  ~(mask);                     \
	tmpVal  |=  ((val) << (offset)) & (mask);\
	CHANGE_ENDIAN_32(tmpVal) ;               \
	gx_iowrite32(tmpVal, reg);               \
}while(0)

#define GX_GET_FEILD_E(reg,mask,val,offset) do {      \
	unsigned int tmpVal  = *(unsigned int*)(reg); \
	CHANGE_ENDIAN_32(tmpVal) ;                    \
	(val) = (tmpVal & (mask)) >> (offset);        \
}while(0)

#define GX_SET_BIT_E(reg,bit) do {               \
	unsigned int tmpVal  = gx_ioread32(reg); \
	CHANGE_ENDIAN_32(tmpVal);                \
	tmpVal |=(1<<(bit));                     \
	CHANGE_ENDIAN_32(tmpVal);                \
	gx_iowrite32(tmpVal, reg);               \
}while(0)

#define GX_CLR_BIT_E(reg,bit) do {               \
	unsigned int tmpVal  = gx_ioread32(reg); \
	CHANGE_ENDIAN_32(tmpVal);                \
	tmpVal  &= (~(1<<(bit)));                \
	CHANGE_ENDIAN_32(tmpVal);                \
	gx_iowrite32(tmpVal, reg);               \
}while(0)

#endif    /* __GXAV_BITOPS_H__ */
