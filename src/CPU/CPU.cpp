#include "CPU/CPU.h"
#include "Instruction/Instructions/LDA.h"
#include "Instruction/Instructions.h"

#include <random>
#include "CPU.h"

CPU::CPU()
{
}

CPU::~CPU()
{
    
}

void CPU::init()
{
    mAddressingModes.reset(new Addressing(shared_from_this()));
    initInstructions();
}

void CPU::reset()
{
    mRegisters.PC = 0xFFCC;
    mRegisters.SP = 0xFF;
    mRegisters.statusRegister.statusFlags.D = 0;
    mRegisters.A = 0;
    mRegisters.X = 0;
    mRegisters.Y = 0;
    mRegisters.statusRegister.byte = 0;
}

void CPU::randomizeRegisters()
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis(0, UINT8_MAX);

    setRegister(Register::A, dis(gen));
    setRegister(Register::X, dis(gen));
    setRegister(Register::Y, dis(gen));
}

uint8_t CPU::fetchByte()
{
    uint8_t byte = mBus->readByte(mRegisters.PC);

    ++mRegisters.PC;

    return byte;
}

uint16_t CPU::fetchWord()
{
    uint16_t word = mBus->readWord(mRegisters.PC);

    ++mRegisters.PC;
    ++mRegisters.PC;

    return word;
}

void CPU::writeByte(uint16_t address, uint8_t byte)
{
    mBus->writeByte(address, byte);
}

void CPU::writeWord(uint16_t address, uint16_t word)
{
    mBus->writeWord(address, word);
}

void CPU::pushByte(uint8_t data)
{
    mBus->writeByte(mStackPointerOffset + mRegisters.SP, data);
    mRegisters.SP--;
}

uint8_t CPU::popByte()
{
    mRegisters.SP++;
    return mBus->readByte(mStackPointerOffset + mRegisters.SP);
}

void CPU::pushWord(uint16_t data)
{
    pushByte((data & 0xFF00) >> 8);
    pushByte(data & 0x00FF);
}

uint16_t CPU::popWord()
{
    uint16_t data = 0;
    uint8_t lo = popByte();
    uint8_t hi = popByte();

    return lo | (hi << 8 & 0xFF00);
}

uint8_t CPU::readByte(uint16_t address)
{
    return mBus->readByte(address);
}

uint16_t CPU::readWord(uint16_t address)
{
    return mBus->readWord(address);
}

void CPU::setFlag(Flag flag, bool value)
{
    mRegisters.statusRegister.setFlag(flag, value);
}

bool CPU::getFlag(Flag flag)
{
    return mRegisters.statusRegister.getFlag(flag);
}

void CPU::setRegister(Register reg, uint16_t value)
{
    switch (reg)
    {
        case PC:
            mRegisters.PC = value;
            break;

        case PS:
            mRegisters.statusRegister.byte = value;
            break;

        case SP:
            mRegisters.SP = value;
            break;

        case A:
            mRegisters.A = value;
            break;

        case X:
            mRegisters.X = value;
            break;

        case Y:
            mRegisters.Y = value;
            break;
        
        default:
            break;
    }
}

uint16_t CPU::getRegister(Register reg)
{
    switch (reg)
    {
        case PC:
            return mRegisters.PC;

        case PS:
            return mRegisters.statusRegister.byte;

        case SP:
            return mRegisters.SP;

        case A:
            return mRegisters.A;

        case X:
            return mRegisters.X;

        case Y:
            return mRegisters.Y;
        
        default:
            return -1;
    }
}

void CPU::execute()
{
    OpCode opCode = static_cast<OpCode>(fetchByte());

    if(mInstructions.count(opCode) == 0)
    {
        return;
    }

    mInstructions.at(opCode)->run();
}

void CPU::randomizeFlags()
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis(0, UINT8_MAX);

    mRegisters.statusRegister.byte = dis(gen);
}

void CPU::connectBus(std::shared_ptr<IBus> bus)
{
    mBus = bus;
}
AddressingData CPU::addressing(AddressingMode mode) const
{
    return mAddressingModes->addressing(mode);
}
const Instruction *CPU::getInstruction(OpCode opcode) const
{
    return mInstructions.at(opcode).get();
}
void CPU::initInstructions()
{
    // ADC
    mInstructions[OC_ADC_IMMEDIATE].reset(new ADC(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_ADC_ZERO_PAGE].reset(new ADC(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_ADC_ZERO_PAGE_X].reset(new ADC(shared_from_this(), AddressingMode::ZeroPageX, 4));
    mInstructions[OC_ADC_ABSOLUTE].reset(new ADC(shared_from_this(), AddressingMode::Absolute, 4));
    mInstructions[OC_ADC_ABSOLUTE_X].reset(new ADC(shared_from_this(), AddressingMode::AbsoluteX, 4));
    mInstructions[OC_ADC_ABSOLUTE_Y].reset(new ADC(shared_from_this(), AddressingMode::AbsoluteY, 4));
    mInstructions[OC_ADC_INDIRECT_X].reset(new ADC(shared_from_this(), AddressingMode::IndirectX, 6));
    mInstructions[OC_ADC_INDIRECT_Y].reset(new ADC(shared_from_this(), AddressingMode::IndirectY, 5));
    // AND
    mInstructions[OC_AND_IMMEDIATE].reset(new AND(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_AND_ZERO_PAGE].reset(new AND(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_AND_ZERO_PAGE_X].reset(new AND(shared_from_this(), AddressingMode::ZeroPageX, 4));
    mInstructions[OC_AND_ABSOLUTE].reset(new AND(shared_from_this(), AddressingMode::Absolute, 4));
    mInstructions[OC_AND_ABSOLUTE_X].reset(new AND(shared_from_this(), AddressingMode::AbsoluteX, 4));
    mInstructions[OC_AND_ABSOLUTE_Y].reset(new AND(shared_from_this(), AddressingMode::AbsoluteY, 4));
    mInstructions[OC_AND_INDIRECT_X].reset(new AND(shared_from_this(), AddressingMode::IndirectX, 6));
    mInstructions[OC_AND_INDIRECT_Y].reset(new AND(shared_from_this(), AddressingMode::IndirectY, 5));
    // ASL
    mInstructions[OC_ASL_ACCUMULATOR].reset(new ASL(shared_from_this(), AddressingMode::Accumulator, 2));
    mInstructions[OC_ASL_ZERO_PAGE].reset(new ASL(shared_from_this(), AddressingMode::ZeroPage, 5));
    mInstructions[OC_ASL_ZERO_PAGE_X].reset(new ASL(shared_from_this(), AddressingMode::ZeroPageX, 6));
    mInstructions[OC_ASL_ABSOLUTE].reset(new ASL(shared_from_this(), AddressingMode::Absolute, 6));
    mInstructions[OC_ASL_ABSOLUTE_X].reset(new ASL(shared_from_this(), AddressingMode::AbsoluteX, 7));
    // BCC
    mInstructions[OC_BCC_RELATIVE].reset(new BCC(shared_from_this(), AddressingMode::Relative, 2));
    // BCS
    mInstructions[OC_BCS_RELATIVE].reset(new BCS(shared_from_this(), AddressingMode::Relative, 2));
    // BIT
    mInstructions[OC_BIT_ZERO_PAGE].reset(new BIT(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_BIT_ABSOLUTE].reset(new BIT(shared_from_this(), AddressingMode::Absolute, 4));
    // BMI
    mInstructions[OC_BMI_RELATIVE].reset(new BMI(shared_from_this(), AddressingMode::Relative, 2));
    // BNE
    mInstructions[OC_BNE_RELATIVE].reset(new BNE(shared_from_this(), AddressingMode::Relative, 2));
    // BPL
    mInstructions[OC_BPL_RELATIVE].reset(new BPL(shared_from_this(), AddressingMode::Relative, 2));
    //BRK
    mInstructions[OC_BRK_IMPLIED].reset(new BRK(shared_from_this()));
    //BVC
    mInstructions[OC_BVC_RELATIVE].reset(new BVC(shared_from_this()));
    //BVS
    mInstructions[OC_BVS_RELATIVE].reset(new BVS(shared_from_this()));
    //CLC
    mInstructions[OC_CLC_IMPLIED].reset(new CLC(shared_from_this()));
    //CLD
    mInstructions[OC_CLD_IMPLIED].reset(new CLD(shared_from_this()));
    //CLI
    mInstructions[OC_CLI_IMPLIED].reset(new CLI(shared_from_this()));
    //CLV   
    mInstructions[OC_CLV_IMPLIED].reset(new CLV(shared_from_this()));
    //CMP
    mInstructions[OC_CMP_IMMEDIATE].reset(new CMP(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_CMP_ZERO_PAGE].reset(new CMP(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_CMP_ZERO_PAGE_X].reset(new CMP(shared_from_this(), AddressingMode::ZeroPageX, 4));
    mInstructions[OC_CMP_ABSOLUTE].reset(new CMP(shared_from_this(), AddressingMode::Absolute, 4));
    mInstructions[OC_CMP_ABSOLUTE_X].reset(new CMP(shared_from_this(), AddressingMode::AbsoluteX, 4));
    mInstructions[OC_CMP_ABSOLUTE_Y].reset(new CMP(shared_from_this(), AddressingMode::AbsoluteY, 4));
    mInstructions[OC_CMP_INDIRECT_X].reset(new CMP(shared_from_this(), AddressingMode::IndirectX, 6));
    mInstructions[OC_CMP_INDIRECT_Y].reset(new CMP(shared_from_this(), AddressingMode::IndirectY, 5));
    //CPX
    mInstructions[OC_CPX_IMMEDIATE].reset(new CPX(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_CPX_ZERO_PAGE].reset(new CPX(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_CPX_ABSOLUTE].reset(new CPX(shared_from_this(), AddressingMode::Absolute, 4));
    //CPY
    mInstructions[OC_CPY_IMMEDIATE].reset(new CPY(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_CPY_ZERO_PAGE].reset(new CPY(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_CPY_ABSOLUTE].reset(new CPY(shared_from_this(), AddressingMode::Absolute, 4));
    //DEC
    mInstructions[OC_DEC_ZERO_PAGE].reset(new DEC(shared_from_this(), AddressingMode::ZeroPage, 2));
    mInstructions[OC_DEC_ZERO_PAGE_X].reset(new DEC(shared_from_this(), AddressingMode::ZeroPageX, 2));
    mInstructions[OC_DEC_ABSOLUTE].reset(new DEC(shared_from_this(), AddressingMode::Absolute, 2));
    mInstructions[OC_DEC_ABSOLUTE_X].reset(new DEC(shared_from_this(), AddressingMode::AbsoluteX, 2));
    //DEX
    mInstructions[OC_DEX_IMPLIED].reset(new DEX(shared_from_this(), AddressingMode::Implied, 2));
    //DEY
    mInstructions[OC_DEY_IMPLIED].reset(new DEY(shared_from_this(), AddressingMode::Implied, 2));
    //EOR
    mInstructions[OC_EOR_IMMEDIATE].reset(new EOR(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_EOR_ZERO_PAGE].reset(new EOR(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_EOR_ZERO_PAGE_X].reset(new EOR(shared_from_this(), AddressingMode::ZeroPageX, 4));
    mInstructions[OC_EOR_ABSOLUTE].reset(new EOR(shared_from_this(), AddressingMode::Absolute, 4));
    mInstructions[OC_EOR_ABSOLUTE_X].reset(new EOR(shared_from_this(), AddressingMode::AbsoluteX, 4));
    mInstructions[OC_EOR_ABSOLUTE_Y].reset(new EOR(shared_from_this(), AddressingMode::AbsoluteY, 4));
    mInstructions[OC_EOR_INDIRECT_X].reset(new EOR(shared_from_this(), AddressingMode::IndirectX, 6));
    mInstructions[OC_EOR_INDIRECT_Y].reset(new EOR(shared_from_this(), AddressingMode::IndirectY, 5));
    //INC
    mInstructions[OC_INC_ZERO_PAGE].reset(new INC(shared_from_this(), AddressingMode::ZeroPage, 5));
    mInstructions[OC_INC_ZERO_PAGE_X].reset(new INC(shared_from_this(), AddressingMode::IndirectX, 6));
    mInstructions[OC_INC_ABSOLUTE].reset(new INC(shared_from_this(), AddressingMode::Absolute, 6));
    mInstructions[OC_INC_ABSOLUTE_X].reset(new INC(shared_from_this(), AddressingMode::AbsoluteX, 6));
    //INX
    mInstructions[OC_INX_IMPLIED].reset(new INX(shared_from_this(), AddressingMode::Implied, 2));
    //INY
    mInstructions[OC_INY_IMPLIED].reset(new INY(shared_from_this(), AddressingMode::Implied, 2));
    //JMP
    mInstructions[OC_JMP_ABSOLUTE].reset(new JMP(shared_from_this(), AddressingMode::Absolute, 3));
    mInstructions[OC_JMP_INDIRECT].reset(new JMP(shared_from_this(), AddressingMode::Indirect, 5));
    //JSR
    mInstructions[OC_JSR_ABSOLUTE].reset(new JSR(shared_from_this()));
    //LDA
    mInstructions[OC_LDA_IMMEDIATE].reset(new LDA(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_LDA_ZERO_PAGE].reset(new LDA(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_LDA_ZERO_PAGE_X].reset(new LDA(shared_from_this(), AddressingMode::ZeroPageX, 4));
    mInstructions[OC_LDA_ABSOLUTE].reset(new LDA(shared_from_this(), AddressingMode::Absolute, 2));
    mInstructions[OC_LDA_ABSOLUTE_X].reset(new LDA(shared_from_this(), AddressingMode::AbsoluteX, 4));
    mInstructions[OC_LDA_ABSOLUTE_Y].reset(new LDA(shared_from_this(), AddressingMode::AbsoluteY, 4));
    mInstructions[OC_LDA_INDIRECT_X].reset(new LDA(shared_from_this(), AddressingMode::IndirectX, 6));
    mInstructions[OC_LDA_INDIRECT_Y].reset(new LDA(shared_from_this(), AddressingMode::IndirectY, 5));
    //LDX
    mInstructions[OC_LDX_IMMEDIATE].reset(new LDX(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_LDX_ZERO_PAGE].reset(new LDX(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_LDX_ZERO_PAGE_Y].reset(new LDX(shared_from_this(), AddressingMode::ZeroPageY, 4));
    mInstructions[OC_LDX_ABSOLUTE].reset(new LDX(shared_from_this(), AddressingMode::Absolute, 4));
    mInstructions[OC_LDX_ABSOLUTE_Y].reset(new LDX(shared_from_this(), AddressingMode::AbsoluteY, 4));
    //LDY
    mInstructions[OC_LDY_IMMEDIATE].reset(new LDY(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_LDY_ZERO_PAGE].reset(new LDY(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_LDY_ZERO_PAGE_X].reset(new LDY(shared_from_this(), AddressingMode::ZeroPageX, 4));
    mInstructions[OC_LDY_ABSOLUTE].reset(new LDY(shared_from_this(), AddressingMode::Absolute, 4));
    mInstructions[OC_LDY_ABSOLUTE_X].reset(new LDY(shared_from_this(), AddressingMode::AbsoluteX, 4));
    //LSR
    mInstructions[OC_LSR_ACCUMULATOR].reset(new LSR(shared_from_this(), AddressingMode::Accumulator, 2));
    mInstructions[OC_LSR_ZERO_PAGE].reset(new LSR(shared_from_this(), AddressingMode::ZeroPage, 5));
    mInstructions[OC_LSR_ZERO_PAGE_X].reset(new LSR(shared_from_this(), AddressingMode::ZeroPageX, 6));
    mInstructions[OC_LSR_ABSOLUTE].reset(new LSR(shared_from_this(), AddressingMode::Absolute, 6));
    mInstructions[OC_LSR_ABSOLUTE_X].reset(new LSR(shared_from_this(), AddressingMode::AbsoluteX, 7));
    //NOP
    mInstructions[OC_NOP_IMPLIED].reset(new NOP(shared_from_this()));
    //ORA
    mInstructions[OC_ORA_IMMEDIATE].reset(new ORA(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_ORA_ZERO_PAGE].reset(new ORA(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_ORA_ZERO_PAGE_X].reset(new ORA(shared_from_this(), AddressingMode::ZeroPageX, 4));
    mInstructions[OC_ORA_ABSOLUTE].reset(new ORA(shared_from_this(), AddressingMode::Absolute, 4));
    mInstructions[OC_ORA_ABSOLUTE_X].reset(new ORA(shared_from_this(), AddressingMode::AbsoluteX, 4));
    mInstructions[OC_ORA_ABSOLUTE_Y].reset(new ORA(shared_from_this(), AddressingMode::AbsoluteY, 4));
    mInstructions[OC_ORA_INDIRECT_X].reset(new ORA(shared_from_this(), AddressingMode::IndirectX, 6));
    mInstructions[OC_ORA_INDIRECT_Y].reset(new ORA(shared_from_this(), AddressingMode::IndirectY, 5));
    //PHA
    mInstructions[OC_PHA_IMPLIED].reset(new PHA(shared_from_this()));
    //PHP
    mInstructions[OC_PHP_IMPLIED].reset(new PHP(shared_from_this()));
    //PLA
    mInstructions[OC_PLA_IMPLIED].reset(new PLA(shared_from_this()));
    //PLP
    mInstructions[OC_PLP_IMPLIED].reset(new PLP(shared_from_this()));
    //ROL
    mInstructions[OC_ROL_ACCUMULATOR].reset(new ROL(shared_from_this(), AddressingMode::Accumulator, 2));
    mInstructions[OC_ROL_ZERO_PAGE].reset(new ROL(shared_from_this(), AddressingMode::ZeroPage, 5));
    mInstructions[OC_ROL_ZERO_PAGE_X].reset(new ROL(shared_from_this(), AddressingMode::ZeroPageX, 6));
    mInstructions[OC_ROL_ABSOLUTE].reset(new ROL(shared_from_this(), AddressingMode::Absolute, 6));
    mInstructions[OC_ROL_ABSOLUTE_X].reset(new ROL(shared_from_this(), AddressingMode::AbsoluteX, 7));
    //ROR
    mInstructions[OC_ROR_ACCUMULATOR].reset(new ROR(shared_from_this(), AddressingMode::Accumulator, 2));
    mInstructions[OC_ROR_ZERO_PAGE].reset(new ROR(shared_from_this(), AddressingMode::ZeroPage, 5));
    mInstructions[OC_ROR_ZERO_PAGE_X].reset(new ROR(shared_from_this(), AddressingMode::ZeroPageX, 6));
    mInstructions[OC_ROR_ABSOLUTE].reset(new ROR(shared_from_this(), AddressingMode::Absolute, 6));
    mInstructions[OC_ROR_ABSOLUTE_X].reset(new ROR(shared_from_this(), AddressingMode::AbsoluteX, 7));
    //RTI
    mInstructions[OC_RTI_IMPLIED].reset(new RTI(shared_from_this()));
    //RTS
    mInstructions[OC_RTS_IMPLIED].reset(new RTS(shared_from_this()));
    //SBC
    mInstructions[OC_SBC_IMMEDIATE].reset(new SBC(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_SBC_ZERO_PAGE].reset(new SBC(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_SBC_ZERO_PAGE_X].reset(new SBC(shared_from_this(), AddressingMode::ZeroPageX, 4));
    mInstructions[OC_SBC_ABSOLUTE].reset(new SBC(shared_from_this(), AddressingMode::Absolute, 4));
    mInstructions[OC_SBC_ABSOLUTE_X].reset(new SBC(shared_from_this(), AddressingMode::AbsoluteX, 4));
    mInstructions[OC_SBC_ABSOLUTE_Y].reset(new SBC(shared_from_this(), AddressingMode::AbsoluteY, 4));
    mInstructions[OC_SBC_INDIRECT_X].reset(new SBC(shared_from_this(), AddressingMode::IndirectX, 6));
    mInstructions[OC_SBC_INDIRECT_Y].reset(new SBC(shared_from_this(), AddressingMode::IndirectY, 5));
    //SEC
    mInstructions[OC_SEC_IMPLIED].reset(new SEC(shared_from_this()));
    //SED
    mInstructions[OC_SED_IMPLIED].reset(new SED(shared_from_this()));
    //SEI
    mInstructions[OC_SEI_IMPLIED].reset(new SEI(shared_from_this()));
    //STA
    mInstructions[OC_SEI_IMPLIED].reset(new SEI(shared_from_this()));
    //STA
    mInstructions[OC_STA_ZERO_PAGE].reset(new STA(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_STA_ZERO_PAGE_X].reset(new STA(shared_from_this(), AddressingMode::ZeroPageX, 4));
    mInstructions[OC_STA_ABSOLUTE].reset(new STA(shared_from_this(), AddressingMode::Absolute, 4));
    mInstructions[OC_STA_ABSOLUTE_X].reset(new STA(shared_from_this(), AddressingMode::AbsoluteX, 4));
    mInstructions[OC_STA_ABSOLUTE_Y].reset(new STA(shared_from_this(), AddressingMode::AbsoluteY, 4));
    mInstructions[OC_STA_INDIRECT_X].reset(new STA(shared_from_this(), AddressingMode::IndirectX, 6));
    mInstructions[OC_STA_INDIRECT_Y].reset(new STA(shared_from_this(), AddressingMode::IndirectY, 5));
    //STX
    mInstructions[OC_STX_ZERO_PAGE].reset(new STX(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_STX_ZERO_PAGE_Y].reset(new STX(shared_from_this(), AddressingMode::ZeroPageY, 4));
    mInstructions[OC_STX_ABSOLUTE].reset(new STX(shared_from_this(), AddressingMode::Absolute, 4));
    //STY
    mInstructions[OC_STY_ZERO_PAGE].reset(new STY(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_STY_ZERO_PAGE_X].reset(new STY(shared_from_this(), AddressingMode::ZeroPageX, 4));
    mInstructions[OC_STY_ABSOLUTE].reset(new STY(shared_from_this(), AddressingMode::Absolute, 4));
    //STA
    mInstructions[OC_TAX_IMPLIED].reset(new TAX(shared_from_this()));
    //STY
    mInstructions[OC_TAY_IMPLIED].reset(new TAY(shared_from_this()));
    //STX
    mInstructions[OC_TSX_IMPLIED].reset(new TSX(shared_from_this()));
    //TXA
    mInstructions[OC_TXA_IMPLIED].reset(new TXA(shared_from_this()));
    //TXS
    mInstructions[OC_TXS_IMPLIED].reset(new TXS(shared_from_this()));
    //TYA
    mInstructions[OC_TXS_IMPLIED].reset(new TYA(shared_from_this()));
}