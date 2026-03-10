// ======================================================================
// \title  TcsSim.cpp
// \author jstar
// \brief  cpp file for TcsSim component implementation class
// ======================================================================

#include "tcs_src/TcsSim.hpp"
#include <Fw/Logger/Logger.hpp>
#include <Fw/Log/LogString.hpp>
// #include "FpConfig.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  TcsSim ::
    TcsSim(const char *const compName) : TcsSimComponentBase(compName)
{
    TcsUart.deviceString = TCS_CFG_STRING;
    TcsUart.handle = TCS_CFG_HANDLE;
    TcsUart.isOpen = PORT_CLOSED;
    TcsUart.baud = TCS_CFG_BAUDRATE_HZ;
    status = uart_init_port(&TcsUart);
    status = uart_close_port(&TcsUart);

    HkTelemetryPkt.DeviceEnabled = TCS_DEVICE_DISABLED;
    HkTelemetryPkt.CommandCount = 0;
    HkTelemetryPkt.CommandErrorCount = 0;
    HkTelemetryPkt.DeviceCount = 0;
    HkTelemetryPkt.DeviceErrorCount = 0;
}
  
  TcsSim ::
    ~TcsSim()
  {
      status = uart_close_port(&TcsUart);
  }

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  void TcsSim :: NOOP_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {

    status = TCS_CommandDevice(&TcsUart, TCS_DEVICE_NOOP_CMD, 0);
    Fw::LogStringArg log_msg("NOOP SENT");
    this->log_ACTIVITY_HI_TELEM(log_msg);
    // OS_printf("NOOP SENT\n");

    this->tlmWrite_CommandCount(++HkTelemetryPkt.CommandCount);

    this->tlmWrite_ReportedComponentCount(TcsHK.DeviceCounter);
    this->tlmWrite_DeviceConfig(TcsHK.DeviceConfig);
    this->tlmWrite_DeviceStatus(TcsHK.DeviceStatus);
    this->tlmWrite_DeviceEnabled(get_active_state(HkTelemetryPkt.DeviceEnabled));

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void TcsSim :: REQUEST_HOUSEKEEPING_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    
    if(HkTelemetryPkt.DeviceEnabled == TCS_DEVICE_ENABLED)
    {
      HkTelemetryPkt.CommandCount++;
      status = TCS_RequestHK(&TcsUart, &TcsHK);
      if (status == OS_SUCCESS)
      {
          HkTelemetryPkt.DeviceCount++;
          Fw::LogStringArg log_msg("RequestHK command success\n");
          this->log_ACTIVITY_HI_TELEM(log_msg);
          // OS_printf("Request Housekeeping Successful\n");
      }
      else
      {
          HkTelemetryPkt.DeviceErrorCount++;
          Fw::LogStringArg log_msg("RequestHK command failed!\n");
          this->log_ACTIVITY_HI_TELEM(log_msg);;
          // OS_printf("Request Housekeeping Failed\n");
      }

    }
    else
    {
      HkTelemetryPkt.CommandErrorCount++;
      Fw::LogStringArg log_msg("RequestHK failed: Device Disabled\n");
      this->log_ACTIVITY_HI_TELEM(log_msg);
      // OS_printf("Request Housekeeping failed, Device Disabled\n");
    }
    
    this->tlmWrite_ReportedComponentCount(TcsHK.DeviceCounter);
    this->tlmWrite_DeviceConfig(TcsHK.DeviceConfig);
    this->tlmWrite_DeviceStatus(TcsHK.DeviceStatus);
    this->tlmWrite_DeviceCount(HkTelemetryPkt.DeviceCount);
    this->tlmWrite_DeviceErrorCount(HkTelemetryPkt.DeviceErrorCount);
    this->tlmWrite_CommandCount(HkTelemetryPkt.CommandCount);
    this->tlmWrite_CommandErrorCount(HkTelemetryPkt.CommandErrorCount);
    this->tlmWrite_DeviceEnabled(get_active_state(HkTelemetryPkt.DeviceEnabled));

    // Tell the fprime command system that we have completed the processing of the supplied command with OK status
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void TcsSim :: TCS_SEQ_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    
  // seq_toggle = 1;
  
    for(int i=0;i<20;i++){
    // while(1){
      sleep(1);
      // printf("seq toggle is equal to %d \n", seq_toggle);
      // if(seq_toggle==0){
      //   break;
      // }

      if(HkTelemetryPkt.DeviceEnabled == TCS_DEVICE_ENABLED)
      {
        HkTelemetryPkt.CommandCount++;
        status = TCS_RequestHK(&TcsUart, &TcsHK);
        if (status == OS_SUCCESS)
        {
            HkTelemetryPkt.DeviceCount++;
            Fw::LogStringArg log_msg("RequestHK command success\n");
            this->log_ACTIVITY_HI_TELEM(log_msg);
        }
        else
        {
            HkTelemetryPkt.DeviceErrorCount++;
            Fw::LogStringArg log_msg("RequestHK command failed!\n");
            this->log_ACTIVITY_HI_TELEM(log_msg);
        }

      }
      else
      {
        HkTelemetryPkt.CommandErrorCount++;
        Fw::LogStringArg log_msg("RequestHK failed: Device Disabled\n");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }
      
      this->tlmWrite_ReportedComponentCount(TcsHK.DeviceCounter);
      this->tlmWrite_DeviceConfig(TcsHK.DeviceConfig);
      this->tlmWrite_DeviceStatus(TcsHK.DeviceStatus);
      this->tlmWrite_DeviceCount(HkTelemetryPkt.DeviceCount);
      this->tlmWrite_DeviceErrorCount(HkTelemetryPkt.DeviceErrorCount);
      this->tlmWrite_CommandCount(HkTelemetryPkt.CommandCount);
      this->tlmWrite_CommandErrorCount(HkTelemetryPkt.CommandErrorCount);
      this->tlmWrite_DeviceEnabled(get_active_state(HkTelemetryPkt.DeviceEnabled));

    }

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void TcsSim :: ENABLE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {

    if(HkTelemetryPkt.DeviceEnabled == TCS_DEVICE_DISABLED)
    {

      HkTelemetryPkt.CommandCount++;
      
      TcsUart.deviceString  = TCS_CFG_STRING;
      TcsUart.handle        = TCS_CFG_HANDLE;
      TcsUart.isOpen        = PORT_CLOSED;
      TcsUart.baud          = TCS_CFG_BAUDRATE_HZ;
      TcsUart.access_option = uart_access_flag_RDWR;

      status = uart_init_port(&TcsUart);
      if(status == OS_SUCCESS)
      {

        HkTelemetryPkt.DeviceEnabled = TCS_DEVICE_ENABLED;
        HkTelemetryPkt.DeviceCount++;
        
        Fw::LogStringArg log_msg("Successfully Enabled");
        this->log_ACTIVITY_HI_TELEM(log_msg); 
        // OS_printf("TcsSim Enable Succeeded\n");  
      }
      else
      {
        HkTelemetryPkt.DeviceErrorCount++;
        Fw::LogStringArg log_msg("Enable failed, failed to init UART port");
        this->log_ACTIVITY_HI_TELEM(log_msg);   
        // OS_printf("TcsSim Enable Failed to init UART port\n");  
      }
    }
    else
    {
      HkTelemetryPkt.CommandErrorCount++;
      Fw::LogStringArg log_msg("Failed, Already Enabled");
      this->log_ACTIVITY_HI_TELEM(log_msg); 
      // OS_printf("TcsSim Enable Failed, Already Enabled\n");
    }

    this->tlmWrite_DeviceCount(HkTelemetryPkt.DeviceCount);
    this->tlmWrite_DeviceErrorCount(HkTelemetryPkt.DeviceErrorCount);
    this->tlmWrite_CommandCount(HkTelemetryPkt.CommandCount);
    this->tlmWrite_CommandErrorCount(HkTelemetryPkt.CommandErrorCount);
    this->tlmWrite_DeviceEnabled(get_active_state(HkTelemetryPkt.DeviceEnabled));
    this->tlmWrite_ReportedComponentCount(TcsHK.DeviceCounter);
    this->tlmWrite_DeviceConfig(TcsHK.DeviceConfig);
    this->tlmWrite_DeviceStatus(TcsHK.DeviceStatus);

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void TcsSim :: DISABLE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {

    if(HkTelemetryPkt.DeviceEnabled == TCS_DEVICE_ENABLED)
    {

      HkTelemetryPkt.CommandCount++;

      status = uart_close_port(&TcsUart);
      if (status == OS_SUCCESS)
      {
        HkTelemetryPkt.DeviceEnabled = TCS_DEVICE_DISABLED;
        HkTelemetryPkt.DeviceCount++;

        Fw::LogStringArg log_msg("Disabled Successfully");
        this->log_ACTIVITY_HI_TELEM(log_msg);  
        // OS_printf("TcsSim Disable Succeeded\n");
      }
      else
      {
        HkTelemetryPkt.DeviceErrorCount++;
        Fw::LogStringArg log_msg("Disable Failed to close UART port");
        this->log_ACTIVITY_HI_TELEM(log_msg);   
        // OS_printf("TcsSim Disable Failed to close UART port\n");
      }
    }
    else
    {
      HkTelemetryPkt.CommandErrorCount++;
      Fw::LogStringArg log_msg("Failed, Already Disabled");
      this->log_ACTIVITY_HI_TELEM(log_msg); 
      // OS_printf("TcsSim Disable Failed, device already disabled\n");
    }

    this->tlmWrite_DeviceCount(HkTelemetryPkt.DeviceCount);
    this->tlmWrite_DeviceErrorCount(HkTelemetryPkt.DeviceErrorCount);
    this->tlmWrite_CommandCount(HkTelemetryPkt.CommandCount);
    this->tlmWrite_CommandErrorCount(HkTelemetryPkt.CommandErrorCount);
    this->tlmWrite_DeviceEnabled(get_active_state(HkTelemetryPkt.DeviceEnabled));
    this->tlmWrite_ReportedComponentCount(TcsHK.DeviceCounter);
    this->tlmWrite_DeviceConfig(TcsHK.DeviceConfig);
    this->tlmWrite_DeviceStatus(TcsHK.DeviceStatus);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void TcsSim :: RESET_COUNTERS_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    HkTelemetryPkt.CommandCount = 0;
    HkTelemetryPkt.CommandErrorCount = 0;
    HkTelemetryPkt.DeviceCount = 0;
    HkTelemetryPkt.DeviceErrorCount = 0;

    this->tlmWrite_DeviceCount(HkTelemetryPkt.DeviceCount);
    this->tlmWrite_DeviceErrorCount(HkTelemetryPkt.DeviceErrorCount);
    this->tlmWrite_CommandCount(HkTelemetryPkt.CommandCount);
    this->tlmWrite_CommandErrorCount(HkTelemetryPkt.CommandErrorCount);

    Fw::LogStringArg log_msg("Counters have been Reset");
    this->log_ACTIVITY_HI_TELEM(log_msg);
    // OS_printf("Counters have been Reset\n");

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);

  }

  void TcsSim :: CONFIGURE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const U32 config){

    status = OS_SUCCESS;

    if(HkTelemetryPkt.DeviceEnabled != TCS_DEVICE_ENABLED)
    {
      status = OS_ERROR;

      HkTelemetryPkt.CommandErrorCount++;

      Fw::LogStringArg log_msg("Configure Failed, Device Disabled");
      this->log_ACTIVITY_HI_TELEM(log_msg);
      // OS_printf("Configure Failed, Device Disabled\n");
    }

    if(config == 0xFFFFFFFF) // 4294967295
    {
      status = OS_ERROR;

      HkTelemetryPkt.CommandErrorCount++;

      Fw::LogStringArg log_msg("Configure Failed, Invalid Configuration");
      this->log_ACTIVITY_HI_TELEM(log_msg);
      // OS_printf("Configure Failed, Invalid Configuration Given\n");
    }

    if(status == OS_SUCCESS)
    {
      HkTelemetryPkt.CommandCount++;

      status = TCS_CommandDevice(&TcsUart, TCS_DEVICE_CFG_CMD, config);
      if(status == OS_SUCCESS)
      {
        HkTelemetryPkt.DeviceCount++;
        Fw::LogStringArg log_msg("Successfully Configured Device");
        this->log_ACTIVITY_HI_TELEM(log_msg);
        // OS_printf("Device Successfully Configured\n");
      }
      else
      {
        HkTelemetryPkt.DeviceErrorCount++;
        Fw::LogStringArg log_msg("Failed to Configure Device");
        this->log_ACTIVITY_HI_TELEM(log_msg);
        // OS_printf("Device Configuration Failed\n");
      }
    }

    this->tlmWrite_DeviceCount(HkTelemetryPkt.DeviceCount);
    this->tlmWrite_DeviceErrorCount(HkTelemetryPkt.DeviceErrorCount);
    this->tlmWrite_CommandCount(HkTelemetryPkt.CommandCount);
    this->tlmWrite_CommandErrorCount(HkTelemetryPkt.CommandErrorCount);
    this->tlmWrite_ReportedComponentCount(TcsHK.DeviceCounter);
    this->tlmWrite_DeviceConfig(TcsHK.DeviceConfig);
    this->tlmWrite_DeviceStatus(TcsHK.DeviceStatus);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);

  }

  inline TcsSim_ActiveState TcsSim :: get_active_state(uint8_t DeviceEnabled)
  {
    TcsSim_ActiveState state;

    if(DeviceEnabled == TCS_DEVICE_ENABLED)
    {
      state.e = TcsSim_ActiveState::ENABLED;
    }
    else
    {
      state.e = TcsSim_ActiveState::DISABLED;
    }

    return state;
  }

  //  void TcsSim :: TCS_SEQ_CANCEL_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    
  //   seq_toggle = 0;
  //   printf("seq toggle is equal to %d\n", seq_toggle);

  //   this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  // }

}