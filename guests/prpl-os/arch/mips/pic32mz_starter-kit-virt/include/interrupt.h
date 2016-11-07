void _irq_register(uint32_t mask, funcptr ptr);
void _irq_handler(uint32_t status, uint32_t cause);
void _irq_mask_set(uint32_t mask);
void _irq_mask_clr(uint32_t mask);
void _exception_handler(uint32_t epc, uint32_t opcode);
