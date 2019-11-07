/*
 *         Copyright (c), NXP Semiconductors Bangalore / India
 *
 *                     (C)NXP Semiconductors
 *       All rights are reserved. Reproduction in whole or in part is
 *      prohibited without the written consent of the copyright owner.
 *  NXP reserves the right to make changes without notice at any time.
 * NXP makes no warranty, expressed, implied or statutory, including but
 * not limited to any implied warranty of merchantability or fitness for any
 * particular purpose, or that the use will not infringe any third party patent,
 * copyright or trademark. NXP must not be liable for any loss or damage
 *                          arising from its use.
 */

/** \file
 * Software ICode Application Component of Reader Library Framework.
 * $Author: nxp99556 $
 * $Revision: 2536 $ (v05.22.00)
 * $Date: 2017-01-02 11:00:20 +0530 (Mon, 02 Jan 2017) $
 *
 * History:
 *  CHu: Generated 02. January 2017
 *
 */

#include <ph_Status.h>
#include <ph_RefDefs.h>
#include <phpalSli15693.h>
#include <phalICode.h>


#ifdef NXPBUILD__PHAL_ICODE_SW

#include "phalICode_Sw.h"
#include "../phalICode_Int.h"

/*
 * Initializes the Icode Software component.
 *
 * Input Parameters:
 *      pDataParams             : Pointer to this layer's parameter structure.
 *      wSizeOfDataParams       : Specifies the size of the data parameter structure.
 *      pPalSli15693DataParams  : Pointer to the parameter structure of the underlying palSli15693 layer.
 *      pCryptoDataParams       : Pointer to the parameter structure of the underlying Crypto layer for encryption / Decryption.
 *      pCryptoRngDataParams    : Pointer to the parameter structure of the underlying Crypto layer for random number generation.
 *      pKeyStoreDataParams     : Pointer to the parameter structure of the underlying Keystore layer.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_Init(phalICode_Sw_DataParams_t * pDataParams, uint16_t wSizeOfDataParams, void * pPalSli15693DataParams, void * pCryptoDataParams,
        void * pCryptoRngDataParams, void * pKeyStoreDataParams)
{
    /* Validate the parameters. */
    PH_ASSERT_NULL_DATA_PARAM(pDataParams, PH_COMP_AL_ICODE);
    PH_ASSERT_NULL_PARAM(pPalSli15693DataParams, PH_COMP_AL_ICODE);

    /* Check the size. */
    if (sizeof(phalICode_Sw_DataParams_t) != wSizeOfDataParams)
    {
        return PH_ADD_COMPCODE_FIXED(PH_ERR_INVALID_DATA_PARAMS, PH_COMP_AL_ICODE);
    }

    /* Initialize the structure members. */
    pDataParams->wId                    = PH_COMP_AL_ICODE | PHAL_ICODE_SW_ID;
    pDataParams->pPalSli15693DataParams = pPalSli15693DataParams;
    pDataParams->pCryptoDataParams      = pCryptoDataParams;
    pDataParams->pCryptoRngDataParams   = pCryptoRngDataParams;
    pDataParams->pKeyStoreDataParams    = pKeyStoreDataParams;
    pDataParams->bBuffering             = PH_ON;

    /* Reset the random number buffer. */
    (void)memset(pDataParams->aRnd_Challenge, 0x00, PHAL_ICODE_RANDOM_NUMBER_SIZE);

    return PH_ERR_SUCCESS;
}


/*
 * Performs a Single block read command. When receiving the Read Single Block command, the VICC shall read the requested block and send
 * back its value in the response. If the Option_flag (bOption = PHAL_ICODE_OPTION_ON) is set in the request, the VICC shall return the block
 * security status, followed by the block value. If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC shall return only the block value.
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                              0x00:   PHAL_ICODE_OPTION_OFF
 *                              0x01:   PHAL_ICODE_OPTION_ON
 *                              0x00:   PHAL_ICODE_OPTION_DEFAULT
 *
 *                              If Option is OFF, block Security Status information is not available. Only block data is available.
 *                              Format will be 4 byte data.
 *                              If Option is ON, both block Security Status information and Block Data is available. Format of the
 *                              response will be Status, 4 byte data
 *      bBlockNo        : Block number from where the data to be read.
 *
 * Output Parameters:
 *      ppData          : Information received from VICC in with respect to bOption parameter information.
 *      pDataLen        : Number of received data bytes.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ReadSingleBlock(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint8_t bBlockNo, uint8_t ** ppData,
        uint16_t * pDataLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ReadSingleBlock(
            pDataParams->pPalSli15693DataParams,
            bOption,
            bBlockNo,
            ppData,
            pDataLen));

    return PH_ERR_SUCCESS;
}

/*
 * Performs a Single block write command. When receiving the Write single block command, the VICC shall write the requested block with the
 * data contained in the request and report the success of the operation in the response. If the Option_flag (bOption = PHAL_ICODE_OPTION_ON)
 * is set in the request, the VICC shall wait for the reception of an EOF from the VCD and upon such reception shall return its response.
 * If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC shall return its response when it has completed the write operation starting
 * after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc (302 us) with a total tolerance of  32/fc and latest after 20 ms upon
 * detection of the rising edge of the EOF of the VCD request.
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                          0x00:   PHAL_ICODE_OPTION_OFF (The VICC shall return its response when it has completed the write operation
 *                                                         starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc
 *                                                         (302 us) with a total tolerance of  32/fc and latest after 20 ms upon detection
 *                                                         of the rising edge of the EOF of the VCD request.)
 *                          0x01:   PHAL_ICODE_OPTION_ON (The VICC shall wait for the reception of an EOF from the VCD and upon such reception
 *                                                        shall return its response.)
 *      bBlockNo        : Block number to which the data should be written.
 *      pData           : Information to be written to the specified block number.
 *      bDataLen        : Number of bytes to be written.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_WriteSingleBlock(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint8_t bBlockNo, uint8_t * pData,
        uint8_t bDataLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_WriteSingleBlock(
            pDataParams->pPalSli15693DataParams,
            bOption,
            bBlockNo,
            pData,
            bDataLen));

    return PH_ERR_SUCCESS;
}

/*
 * Performs a Lock block command. When receiving the Lock block command, the VICC shall lock permanently the requested block. If the
 * Option_flag (bOption = PHAL_ICODE_OPTION_ON) is set in the request, the VICC shall wait for the reception of an EOF from the VCD
 * and upon such reception shall return its response. If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC shall return its
 * response when it has completed the lock operation starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc
 * (302 us) with a total tolerance of  32/fc and latest after 20 ms upon detection of the rising edge of the EOF of the VCD request.
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                          0x00:   PHAL_ICODE_OPTION_OFF (The VICC shall return its response when it has completed the lock operation
 *                                                         starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc
 *                                                         (302 us) with a total tolerance of  32/fc and latest after 20 ms upon detection
 *                                                         of the rising edge of the EOF of the VCD request.)
 *                          0x01:   PHAL_ICODE_OPTION_ON (The VICC shall wait for the reception of an EOF from the VCD and upon such reception
 *                                                        shall return its response.)
 *      bBlockNo        : Block number which should be locked.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_LockBlock(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint8_t bBlockNo)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_LockBlock(
            pDataParams->pPalSli15693DataParams,
            bOption,
            bBlockNo));

    return PH_ERR_SUCCESS;
}

/*
 * Performs a Multiple block read command. When receiving the Read Multiple Block command, the VICC shall read the requested block(s) and send
 * back its value in the response. If the Option_flag (bOption = PHAL_ICODE_OPTION_ON) is set in the request, the VICC shall return the block
 * security status, followed by the block value sequentially block by block. If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC shall
 *  return only the block value.
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                          0x00:   PHAL_ICODE_OPTION_OFF (Block Security Status information is not available. Only block data is available.)
 *                          0x01:   PHAL_ICODE_OPTION_ON (Both Block Security Status information and Block Data is available. This will be
 *                                                        available in the first byte of ppData buffer.)
 *      bBlockNo        : Block number from where the data to be read.
 *      bNumBlocks      : Total number of block to read.
 *
 * Output Parameters:
 *      pData           : Information received from VICC in with respect to bOption parameter information.
 *      pDataLen        : Number of received data bytes.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ReadMultipleBlocks(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint8_t bBlockNo, uint8_t bNumBlocks,
        uint8_t * pData, uint16_t * pDataLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ReadMultipleBlocks(
            pDataParams->pPalSli15693DataParams,
            pDataParams->bBuffering,
            bOption,
            bBlockNo,
            bNumBlocks,
            pData,
            pDataLen));

    return PH_ERR_SUCCESS;
}


/*
 * Performs a WriteAFI command. When receiving the Write AFI request, the VICC shall write the  AFI value into its memory.
 * If the  Option_flag (bOption = PHAL_ICODE_OPTION_ON) is set in the request, the VICC shall wait for the reception of an EOF
 * from the VCD and upon such reception shall return its response. If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC
 * shall return its response when it has completed the write operation starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a
 * multiple of 4096/fc (302 us) with a total tolerance of  32/fc and latest after 20 ms upon detection of the rising edge of the
 * EOF of the VCD request.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                          0x00:   PHAL_ICODE_OPTION_OFF (The VICC shall return its response when it has completed the write operation
 *                                                         starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc
 *                                                         (302 us) with a total tolerance of  32/fc and latest after 20 ms upon detection
 *                                                         of the rising edge of the EOF of the VCD request.)
 *                          0x01:   PHAL_ICODE_OPTION_ON (The VICC shall wait for the reception of an EOF from the VCD and upon such reception
 *                                                        shall return its response.)
 *      bAfi            : Value of Application Family Identifier.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_WriteAFI(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint8_t bAfi)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_WriteAFI(
            pDataParams->pPalSli15693DataParams,
            bOption,
            bAfi));

    return PH_ERR_SUCCESS;
}

/*
 * Performs a LockAFI command. When receiving the Lock AFI request, the VICC shall lock the AFI value permanently into its memory.
 * If the  Option_flag (bOption = PHAL_ICODE_OPTION_ON) is set in the request, the VICC shall wait for the reception of an EOF
 * from the VCD and upon such reception shall return its response. If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC
 * shall return its response when it has completed the lock operation starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a
 * multiple of 4096/fc (302 us) with a total tolerance of  32/fc and latest after 20 ms upon detection of the rising edge of the
 * EOF of the VCD request.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                          0x00:   PHAL_ICODE_OPTION_OFF (The VICC shall return its response when it has completed the lock operation
 *                                                         starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc
 *                                                         (302 us) with a total tolerance of  32/fc and latest after 20 ms upon detection
 *                                                         of the rising edge of the EOF of the VCD request.)
 *                          0x01:   PHAL_ICODE_OPTION_ON (The VICC shall wait for the reception of an EOF from the VCD and upon such reception
 *                                                        shall return its response.)
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_LockAFI(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_LockAFI(
            pDataParams->pPalSli15693DataParams,
            bOption));

    return PH_ERR_SUCCESS;
}

/*
 * Performs WriteDSFID command. When receiving the Write DSFID request, the VICC shall write the DSFID value into its memory.
 * If the  Option_flag (bOption = PHAL_ICODE_OPTION_ON) is set in the request, the VICC shall wait for the reception of an EOF
 * from the VCD and upon such reception shall return its response. If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC
 * shall return its response when it has completed the write operation starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a
 * multiple of 4096/fc (302 us) with a total tolerance of  32/fc and latest after 20 ms upon detection of the rising edge of the
 * EOF of the VCD request.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                          0x00:   PHAL_ICODE_OPTION_OFF (The VICC shall return its response when it has completed the write operation
 *                                                         starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc
 *                                                         (302 us) with a total tolerance of  32/fc and latest after 20 ms upon detection
 *                                                         of the rising edge of the EOF of the VCD request.)
 *                          0x01:   PHAL_ICODE_OPTION_ON (The VICC shall wait for the reception of an EOF from the VCD and upon such reception
 *                                                        shall return its response.)
 *      bDsfid          : Value of DSFID (data storage format identifier).
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_WriteDSFID(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint8_t bDsfid)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_WriteDSFID(
            pDataParams->pPalSli15693DataParams,
            bOption,
            bDsfid));

    return PH_ERR_SUCCESS;
}

/*
 * Performs LockDSFID command. When receiving the Lock DSFID request, the VICC shall lock the DSFID value permanently into its memory.
 * If the  Option_flag (bOption = PHAL_ICODE_OPTION_ON) is set in the request, the VICC shall wait for the reception of an EOF from the
 * VCD and upon such reception shall return its response. If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC shall return its
 * response when it has completed the lock operation starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc (302 us)
 * with a total tolerance of  32/fc and latest after 20 ms upon detection of the rising edge of the EOF of the VCD request.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                          0x00:   PHAL_ICODE_OPTION_OFF (The VICC shall return its response when it has completed the lock operation
 *                                                         starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc
 *                                                         (302 us) with a total tolerance of  32/fc and latest after 20 ms upon detection
 *                                                         of the rising edge of the EOF of the VCD request.)
 *                          0x01:   PHAL_ICODE_OPTION_ON (The VICC shall wait for the reception of an EOF from the VCD and upon such reception
 *                                                        shall return its response.)
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_LockDSFID(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_LockDSFID(
            pDataParams->pPalSli15693DataParams,
            bOption));

    return PH_ERR_SUCCESS;
}

/*
 * Performs GetSystemInformation command. This command allows for retrieving the system information value from the VICC.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *
 * Output Parameters:
 *      ppSystemInfo    : The system information of the VICC.
 *      pSystemInfoLen  : Number of received data bytes.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_GetSystemInformation(phalICode_Sw_DataParams_t * pDataParams, uint8_t ** ppSystemInfo, uint16_t * pSystemInfoLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_GetSystemInformation(
            pDataParams->pPalSli15693DataParams,
            ppSystemInfo,
            pSystemInfoLen));

    return PH_ERR_SUCCESS;
}

/*
 * Performs GetMultipleBlockSecurityStatus. When receiving the Get multiple block security status command, the VICC
 * shall send back the block security status.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bBlockNo        : Block number for which the status should be returned.
 *      bNoOfBlocks     : Number of blocks to be used for returning the status.
 *
 * Output Parameters:
 *      pStatus         : The status of the block number mentioned in bBlockNo until bNoOfBlocks.
 *      pStatusLen      : Number of received data bytes.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_GetMultipleBlockSecurityStatus(phalICode_Sw_DataParams_t * pDataParams, uint8_t bBlockNo, uint8_t bNoOfBlocks,
        uint8_t * pStatus, uint16_t * pStatusLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_GetMultipleBlockSecurityStatus(
            pDataParams->pPalSli15693DataParams,
            pDataParams->bBuffering,
            bBlockNo,
            bNoOfBlocks,
            pStatus,
            pStatusLen));

    return PH_ERR_SUCCESS;
}

/*
 * Performs a Multiple block fast read command. When receiving the Read Multiple Block command, the VICC shall read the requested block(s) and
 * send back its value in the response. If the Option_flag (bOption = PHAL_ICODE_OPTION_ON) is set in the request, the VICC shall return the block
 * security status, followed by the block value sequentially block by block. If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC shall
 * return only the block value.
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                          0x00:   PHAL_ICODE_OPTION_OFF (Block Security Status information is not available. Only block data is available.)
 *                          0x01:   PHAL_ICODE_OPTION_ON (Both Block Security Status information and Block Data is available. This will be
 *                                                        available in the first byte of ppData buffer.)
 *      bBlockNo        : Block number from where the data to be read.
 *      bNumBlocks      : Total number of block to read.
 *
 * Output Parameters:
 *      pData           : Information received from VICC in with respect to bOption parameter information.
 *      pDataLen        : Number of received data bytes.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_FastReadMultipleBlocks(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint8_t bBlockNo, uint8_t bNumBlocks,
        uint8_t * pData, uint16_t * pDataLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_FastReadMultipleBlocks(
            pDataParams->pPalSli15693DataParams,
            pDataParams->bBuffering,
            bOption,
            bBlockNo,
            bNumBlocks,
            pData,
            pDataLen));

    return PH_ERR_SUCCESS;
}

/**
 * \brief Performs a Extended Single block read command. When receiving the Extended Read Single Block command, the VICC shall read the
 * requested block and send back its value in the response. If a VICC supports Extended read single block command, it shall also support
 * Read single block command for the first 256 blocks of memory. If the Option_flag (bOption = #PHAL_ICODE_OPTION_ON) is set in the request,
 * the VICC shall return the block security status, followed by the block value. If it is not set (bOption = #PHAL_ICODE_OPTION_OFF), the
 * VICC shall return only the block value.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                              0x00:   PHAL_ICODE_OPTION_OFF
 *                              0x01:   PHAL_ICODE_OPTION_ON
 *                              0x00:   PHAL_ICODE_OPTION_DEFAULT
 *
 *                              If Option is OFF, block Security Status information is not available. Only block data is available.
 *                              Format will be 4 byte data.
 *                              If Option is ON, both block Security Status information and Block Data is available. Format of the
 *                              response will be Status, 4 byte data
 *      wBlockNo        : Block number from where the data to be read.
 *
 * Output Parameters:
 *      ppData          : Information received from VICC in with respect to bOption parameter information.
 *      pDataLen        : Number of received data bytes.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ExtendedReadSingleBlock(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint16_t wBlockNo, uint8_t ** ppData,
        uint16_t * pDataLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ExtendedReadSingleBlock(
            pDataParams->pPalSli15693DataParams,
            bOption,
            wBlockNo,
            ppData,
            pDataLen));

    return PH_ERR_SUCCESS;
}

/**
 * \brief Performs a Extended Single block Write command. When receiving the Extended write single block command, the VICC shall write the
 * requested block with the data contained in the request and report the success of the operation in the response. If a VICC supports
 * Extended write single block command, it shall also support Write single block command for the first 256 blocks of memory.
 *
 * If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC shall return its response when it has completed the write operation starting
 * after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc (302 us) with a total tolerance of  32/fc and latest after 20 ms upon
 * detection of the rising edge of the EOF of the VCD request.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                          0x00:   PHAL_ICODE_OPTION_OFF (The VICC shall return its response when it has completed the write operation
 *                                                         starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc
 *                                                         (302 us) with a total tolerance of  32/fc and latest after 20 ms upon detection
 *                                                         of the rising edge of the EOF of the VCD request.)
 *                          0x01:   PHAL_ICODE_OPTION_ON (The VICC shall wait for the reception of an EOF from the VCD and upon such reception
 *                                                        shall return its response.)
 *      wBlockNo        : Block number to which the data should be written.
 *      pData           : Information to be written to the specified block number.
 *      bDataLen        : Number of bytes to be written.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ExtendedWriteSingleBlock(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint16_t wBlockNo,
        uint8_t * pData, uint8_t bDataLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ExtendedWriteSingleBlock(
            pDataParams->pPalSli15693DataParams,
            bOption,
            wBlockNo,
            pData,
            bDataLen));

    return PH_ERR_SUCCESS;
}

/*
 * Performs a Extended Lock block command. When receiving the Lock block command, the VICC shall lock permanently the requested
 * block. If a VICC supports Extended lock block command, it shall also support Lock block command for the first 256 blocks of memory.
 * If the Option_flag (bOption = PHAL_ICODE_OPTION_ON) is set in the request, the VICC shall wait for the reception of an EOF from the
 * VCD and upon such reception shall return its response. If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC shall return its
 * response when it has completed the lock operation starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc
 * (302 us) with a total tolerance of 32/fc and latest after 20 ms upon detection of the rising edge of the EOF of the VCD request.
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                          0x00:   PHAL_ICODE_OPTION_OFF (The VICC shall return its response when it has completed the lock operation
 *                                                         starting after t1nom [4352/fc (320,9 us), see 9.1.1] + a multiple of 4096/fc
 *                                                         (302 us) with a total tolerance of  32/fc and latest after 20 ms upon detection
 *                                                         of the rising edge of the EOF of the VCD request.)
 *                          0x01:   PHAL_ICODE_OPTION_ON (The VICC shall wait for the reception of an EOF from the VCD and upon such reception
 *                                                        shall return its response.)
 *      wBlockNo        : Block number which should be locked.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ExtendedLockBlock(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint16_t wBlockNo)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ExtendedLockBlock(
            pDataParams->pPalSli15693DataParams,
            bOption,
            wBlockNo));

    return PH_ERR_SUCCESS;
}

/*
 * Performs a Extended Multiple block read command. When receiving the Read Multiple Block command, the VICC shall read the requested block(s)
 * and send back its value in the response. If a VICC supports Extended read multiple blocks command, it shall also support Read multiple blocks
 * command for the first 256 blocks of memory.
 *
 * If the Option_flag (bOption = PHAL_ICODE_OPTION_ON) is set in the request, the VICC shall return the block security status, followed by the block
 * value sequentially block by block. If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC shall return only the block value.
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                          0x00:   PHAL_ICODE_OPTION_OFF (Block Security Status information is not available. Only block data is available.)
 *                          0x01:   PHAL_ICODE_OPTION_ON (Both Block Security Status information and Block Data is available. This will be
 *                                                        available in the first byte of ppData buffer.)
 *      wBlockNo        : Block number from where the data to be read.
 *      wNumBlocks      : Total number of block to read.
 *
 * Output Parameters:
 *      pData           : Information received from VICC in with respect to bOption parameter information.
 *      pDataLen        : Number of received data bytes.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ExtendedReadMultipleBlocks(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint16_t wBlockNo, uint16_t wNumBlocks,
        uint8_t * pData, uint16_t * pDataLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ExtendedReadMultipleBlocks(
            pDataParams->pPalSli15693DataParams,
            pDataParams->bBuffering,
            bOption,
            wBlockNo,
            wNumBlocks,
            pData,
            pDataLen));

    return PH_ERR_SUCCESS;
}


/*
 * Performs ExtendedGetSystemInformation command. This command allows for retrieving the system information value
 * from the VICC and shall be supported by the VICC if extended memory or security functionalities are supported
 * by the VICC.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      bInfoParams         : Extend Get System Information parameter request fields.
 *                              0x10: PHAL_ICODE_INFO_PARAMS_REQUEST_DEFAULT
 *                              0x01: PHAL_ICODE_INFO_PARAMS_REQUEST_DSFID
 *                              0x02: PHAL_ICODE_INFO_PARAMS_REQUEST_AFI
 *                              0x04: PHAL_ICODE_INFO_PARAMS_REQUEST_VICC_MEM_SIZE
 *                              0x08: PHAL_ICODE_INFO_PARAMS_REQUEST_IC_REFERENCE
 *                              0x10: PHAL_ICODE_INFO_PARAMS_REQUEST_MOI
 *                              0x20: PHAL_ICODE_INFO_PARAMS_REQUEST_COMMAND_LIST
 *                              0x50: PHAL_ICODE_INFO_PARAMS_REQUEST_CSI_INFORMATION
 *                              0x80: PHAL_ICODE_INFO_PARAMS_REQUEST_EXT_GET_SYS_INFO
 *
 * Output Parameters:
 *      ppSystemInfo        : The system information of the VICC.
 *      pSystemInfoLen      : Number of received data bytes.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ExtendedGetSystemInformation(phalICode_Sw_DataParams_t * pDataParams, uint8_t bInfoParams, uint8_t ** ppSystemInfo,
        uint16_t * pSystemInfoLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ExtendedGetSystemInformation(
            pDataParams->pPalSli15693DataParams,
            bInfoParams,
            ppSystemInfo,
            pSystemInfoLen));

    return PH_ERR_SUCCESS;
}

/*
 * Performs ExtendedGetMultipleBlockSecurityStatus. When receiving the Extended Get multiple block security status
 * command, the VICC shall send back the block security status. The blocks are numbered from 0000 to FFFF (0 - 65535).
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      wBlockNo        : Block number for which the status should be returned.
 *      wNoOfBlocks     : Number of blocks to be used for returning the status.
 *
 * Output Parameters:
 *      pStatus         : The status of the block number mentioned in wBlockNo until wNoOfBlocks.
 *      pStatusLen      : Number of received data bytes.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ExtendedGetMultipleBlockSecurityStatus(phalICode_Sw_DataParams_t * pDataParams, uint16_t wBlockNo, uint16_t wNoOfBlocks,
        uint8_t * pStatus, uint16_t * pStatusLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ExtendedGetMultipleBlockSecurityStatus(
            pDataParams->pPalSli15693DataParams,
            pDataParams->bBuffering,
            wBlockNo,
            wNoOfBlocks,
            pStatus,
            pStatusLen));

    return PH_ERR_SUCCESS;
}

/*
 * Performs a Extended Multiple block fast read command. When receiving the Read Multiple Block command, the VICC shall read the requested block(s)
 * and send back its value in the response. If a VICC supports Extended read multiple blocks command, it shall also support Read multiple blocks
 * command for the first 256 blocks of memory.
 *
 * If the Option_flag (bOption = PHAL_ICODE_OPTION_ON) is set in the request, the VICC shall return the block security status, followed by the block
 * value sequentially block by block. If it is not set (bOption = PHAL_ICODE_OPTION_OFF), the VICC shall return only the block value.
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *      bOption         : Option flag.
 *                          0x00:   PHAL_ICODE_OPTION_OFF (Block Security Status information is not available. Only block data is available.)
 *                          0x01:   PHAL_ICODE_OPTION_ON (Both Block Security Status information and Block Data is available. This will be
 *                                                        available in the first byte of ppData buffer.)
 *      wBlockNo        : Block number from where the data to be read.
 *      wNumBlocks      : Total number of block to read.
 *
 * Output Parameters:
 *      pData           : Information received from VICC in with respect to bOption parameter information.
 *      pDataLen        : Number of received data bytes.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ExtendedFastReadMultipleBlocks(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint16_t wBlockNo, uint16_t wNumBlocks,
        uint8_t * pData, uint16_t * pDataLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ExtendedFastReadMultipleBlocks(
            pDataParams->pPalSli15693DataParams,
            pDataParams->bBuffering,
            bOption,
            wBlockNo,
            wNumBlocks,
            pData,
            pDataLen));

    return PH_ERR_SUCCESS;
}


/*
 * This command enables the EAS mode if the EAS mode is not locked. If the EAS mode is password protected
 * the EAS password has to be transmitted before with \ref phalICode_SetPassword.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pPalSli15693DataParams  : Pointer to the parameter structure of the underlying palSli15693 layer.
 *      bOption             : Options to be enabled or disabled. As per ISO15693 protocol
 *                              0x00:   PHAL_ICODE_OPTION_OFF Disable option.
 *                              0x01:   PHAL_ICODE_OPTION_ON Enable option.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_SetEAS(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_SetEAS(pDataParams->pPalSli15693DataParams, bOption));

    return PH_ERR_SUCCESS;
}

/*
 * This command disables the EAS mode if the EAS mode is not locked. If the EAS mode is password protected
 * the EAS password has to be transmitted before with \ref phalICode_SetPassword.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      bOption             : Options to be enabled or disabled. As per ISO15693 protocol
 *                              0x00:   PHAL_ICODE_OPTION_OFF Disable option.
 *                              0x01:   PHAL_ICODE_OPTION_ON Enable option.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ResetEAS(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ResetEAS(pDataParams->pPalSli15693DataParams, bOption));

    return PH_ERR_SUCCESS;
}

/*
 * This command locks the current state of the EAS mode and the EAS ID. If the EAS mode is password protected
 * the EAS password has to be transmitted before with \ref phalICode_SetPassword.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      bOption             : Options to be enabled or disabled. As per ISO15693 protocol
 *                              0x00:   PHAL_ICODE_OPTION_OFF Disable option.
 *                              0x01:   PHAL_ICODE_OPTION_ON Enable option.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_LockEAS(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_LockEAS(pDataParams->pPalSli15693DataParams, bOption));

    return PH_ERR_SUCCESS;
}

/*
 * This command returns the EAS sequence if the EAS mode is enabled.
 *
 * bOption disabled: bEasIdMaskLength and pEasIdValue are not transmitted, EAS Sequence is returned;
 * bOption enabled and bEasIdMaskLength = 0: EAS ID is returned;
 * bOption enabled and bEasIdMaskLength > 0: EAS Sequence is returned by ICs with matching pEasIdValue;
 *
 * If the EAS mode is disabled, the label remains silent.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      bOption             : Option flag;
 *                              PHAL_ICODE_OPTION_OFF
 *                                  EAS ID mask length and EAS ID value shall not be transmitted.
 *                                  If the EAS mode is enabled, the EAS response is returned from the ICODE IC.
 *                                  This configuration is compliant with the EAS command of the ICODE IC
 *                              PHAL_ICODE_OPTION_ON.
 *                                  Within the command the EAS ID mask length has to be transmitted to identify how
 *                                  many bits of the following EAS ID value are valid (multiple of 8-bits). Only those
 *                                  ICODE ICs will respond with the EAS sequence which have stored the corresponding
 *                                  data in the EAS ID configuration (selective EAS) and if the EAS Mode is set.
 *                                  If the EAS ID mask length is set to 0, the ICODE IC will answer with its EAS ID
 *      pEasIdValue         : EAS ID; 0, 8 or 16 bits; optional.
 *      bEasIdMaskLen       : 8 bits; optional.
 *
 * Input Parameters:
 *      ppEas               : EAS ID (16 bits) or EAS Sequence (256 bits).
 *      pEasLen             : Length of bytes available in ppEas buffer.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_EASAlarm(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint8_t * pEasIdValue, uint8_t bEasIdMaskLen,
        uint8_t ** ppEas, uint16_t * pEasLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_EASAlarm(
            pDataParams->pPalSli15693DataParams,
            bOption,
            pEasIdValue,
            bEasIdMaskLen,
            ppEas,
            pEasLen));

    return PH_ERR_SUCCESS;
}

/*
 * This command enables the password protection for EAS. The EAS password has to be transmitted before with
 * \ref phalICode_SetPassword.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_PasswordProtectEAS(phalICode_Sw_DataParams_t * pDataParams)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_PasswordProtectEAS(pDataParams->pPalSli15693DataParams));

    return PH_ERR_SUCCESS;
}

/*
 * This command enables the password protection for AFI. The AFI password has to be transmitted before with
 * \ref phalICode_SetPassword.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_PasswordProtectAFI(phalICode_Sw_DataParams_t * pDataParams)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_PasswordProtectAFI(pDataParams->pPalSli15693DataParams));

    return PH_ERR_SUCCESS;
}

/*
 * With this command, a new EAS identifier is stored in the corresponding configuration memory. If the EAS mode
 * is password protected the EAS password has to be transmitted before with \ref phalICode_SetPassword.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      pEasIdValue         : EAS ID; 16 bits.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_WriteEASID(phalICode_Sw_DataParams_t * pDataParams, uint8_t * pEasIdValue)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_WriteEASID(pDataParams->pPalSli15693DataParams, pEasIdValue));

    return PH_ERR_SUCCESS;
}

/*
 * On this command, the label will respond with it's EPC data.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *
 * Output Parameters:
 *      ppEpc               : EPC data; 96 bits.
 *      pEpcLen             : Length of bytes available in ppEpc buffer.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ReadEPC(phalICode_Sw_DataParams_t * pDataParams, uint8_t ** ppEpc, uint16_t * pEpcLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ReadEPC(pDataParams->pPalSli15693DataParams, ppEpc, pEpcLen));

    return PH_ERR_SUCCESS;
}

/*
 * Performs GetNXPSystemInformation command. This command allows for retrieving the NXP system information value from the VICC.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams     : Pointer to this layer's parameter structure.
 *
 * Output Parameters:
 *      ppSystemInfo    : The NXP system information of the VICC.
 *      pSystemInfoLen  : Number of received data bytes.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_GetNXPSystemInformation(phalICode_Sw_DataParams_t * pDataParams, uint8_t ** ppSystemInfo, uint16_t * pSystemInfoLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_GetNXPSystemInformation(
            pDataParams->pPalSli15693DataParams,
            ppSystemInfo,
            pSystemInfoLen));

    return PH_ERR_SUCCESS;
}

/*
 * Performs a GetRandomNumber command. On this command, the label will respond with a random number.
 * The received random number shall be used to diversify the password for the \ref phalICode_SetPassword command.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *
 * Output Parameters:
 *      ppRnd               : Random number; 16 bits.
 *      ppRnd               : Number of bytes in ppRnd buffer.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_GetRandomNumber(phalICode_Sw_DataParams_t * pDataParams, uint8_t ** ppRnd, uint16_t * pRndLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_GetRandomNumber(pDataParams->pPalSli15693DataParams, ppRnd, pRndLen));

    return PH_ERR_SUCCESS;
}

/*
 * Perforns SetPassword command. With this command the different passwords can be transmitted to the label.
 *
 * This command has to be executed just once for the related passwords if the label is powered.
 *
 * \verbatim
 * [XOR password calculation example]
 * pXorPwd[0] = pPassword[0] ^ pRnd[0];
 * pXorPwd[1] = pPassword[1] ^ pRnd[1];
 * pXorPwd[2] = pPassword[2] ^ pRnd[0];
 * pXorPwd[3] = pPassword[3] ^ pRnd[1];
 * \endverbatim
 *
 * \b Remark: This command can only be executed in addressed or selected mode except of Privay Password.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      bPwdIdentifier      : Password Identifier.
 *                              PHAL_ICODE_SET_PASSWORD_READ
 *                              PHAL_ICODE_SET_PASSWORD_WRITE
 *                              PHAL_ICODE_SET_PASSWORD_PRIVACY
 *                              PHAL_ICODE_SET_PASSWORD_DESTROY
 *                              PHAL_ICODE_SET_PASSWORD_EAS
 *      pXorPwd             : XOR Password; 32 bits.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_SetPassword(phalICode_Sw_DataParams_t * pDataParams, uint8_t bPwdIdentifier, uint8_t * pXorPwd)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_SetPassword(pDataParams->pPalSli15693DataParams, bPwdIdentifier, pXorPwd));

    return PH_ERR_SUCCESS;
}

/*
 * Performs WritePassword command. With this command, a new password is written into the related memory. Note that the
 * old password has to be transmitted before with \ref phalICode_SetPassword. The new password takes effect immediately which
 * means that the new password has to be transmitted with \ref phalICode_SetPassword to get access to protected blocks/pages.
 * \b Remark: This command can only be executed in addressed or selected mode.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      bPwdIdentifier      : Password Identifier.
 *                              PHAL_ICODE_SET_PASSWORD_READ
 *                              PHAL_ICODE_SET_PASSWORD_WRITE
 *                              PHAL_ICODE_SET_PASSWORD_PRIVACY
 *                              PHAL_ICODE_SET_PASSWORD_DESTROY
 *                              PHAL_ICODE_SET_PASSWORD_EAS
 *      pPwd                : Plain Password; 32 bits
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_WritePassword(phalICode_Sw_DataParams_t * pDataParams, uint8_t bPwdIdentifier, uint8_t * pPwd)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_WritePassword(pDataParams->pPalSli15693DataParams, bPwdIdentifier, pPwd));

    return PH_ERR_SUCCESS;
}

/*
 * Performs LockPassword command. This command locks the addressed password. Note that the addressed password
 * has to be transmitted before with \ref phalICode_SetPassword. A locked password can not be changed any longer.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      bPwdIdentifier      : Password Identifier.
 *                              PHAL_ICODE_SET_PASSWORD_READ
 *                              PHAL_ICODE_SET_PASSWORD_WRITE
 *                              PHAL_ICODE_SET_PASSWORD_PRIVACY
 *                              PHAL_ICODE_SET_PASSWORD_DESTROY
 *                              PHAL_ICODE_SET_PASSWORD_EAS
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_LockPassword(phalICode_Sw_DataParams_t * pDataParams, uint8_t bPwdIdentifier)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_LockPassword(pDataParams->pPalSli15693DataParams, bPwdIdentifier));

    return PH_ERR_SUCCESS;
}

/*
 * Performs Page protection command. This command changes the protection status of a page. Note that the related
 * passwords have to be transmitted before with \ref phalICode_SetPassword if the page is not public.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      bPPAdd_PageNo       : Page number to be protected in case of products that do not have pages
 *                            charactersized as high and Low.
 *                            Block number to be protected in case of products that have pages
 *                            charactersized as high and Low.
 *      bProtectionStatus   : Protection status options for the products that do not have pages
 *                            charactersized as high and Low.
 *                              0x00: PHAL_ICODE_PROTECT_PAGE_PUBLIC
 *                              0x01: PHAL_ICODE_PROTECT_PAGE_READ_WRITE_READ_PASSWORD
 *                              0x10: PHAL_ICODE_PROTECT_PAGE_WRITE_PASSWORD
 *                              0x11: PHAL_ICODE_PROTECT_PAGE_READ_WRITE_PASSWORD_SEPERATE
 *
 *                            Extended Protection status options for the products that have pages
 *                            charactersized as high and Low.
 *                              0x01: PHAL_ICODE_PROTECT_PAGE_READ_LOW
 *                              0x02: PHAL_ICODE_PROTECT_PAGE_WRITE_LOW
 *                              0x10: PHAL_ICODE_PROTECT_PAGE_READ_HIGH
 *                              0x20: PHAL_ICODE_PROTECT_PAGE_WRITE_HIGH
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ProtectPage(phalICode_Sw_DataParams_t * pDataParams, uint8_t bPPAdd_PageNo, uint8_t bProtectionStatus)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ProtectPage(pDataParams->pPalSli15693DataParams, bPPAdd_PageNo, bProtectionStatus));

    return PH_ERR_SUCCESS;
}

/*
 * Perform LockPageProtectionCondition command. This command permanenty locks the protection status of a page.
 * Note that the related passwords have to be transmitted before with \ref phalICode_SetPassword if the page is
 * not public.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      bPageNo             : Page number to be protected.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_LockPageProtectionCondition(phalICode_Sw_DataParams_t * pDataParams, uint8_t bPageNo)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_LockPageProtectionCondition(pDataParams->pPalSli15693DataParams, bPageNo));

    return PH_ERR_SUCCESS;
}

/*
 * Perform GetMultipleBlockProtectionStatus command. This instructs the label to return the block protection
 * status of the requested blocks.
 *
 * Remark: If bBlockNo + bNoOfBlocks exceeds the total available number of user blocks, the number of received
 * status bytes is less than the requested number. This means that the last returned status byte corresponds to the
 * highest available user block.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      bBlockNo            : First Block number.
 *      bNoOfBlocks         : Number of blocks.
 *
 * Output Parameters:
 *      pProtectionStates   : Protection states of requested blocks.
 *      pNumReceivedStates  : Number of received block protection states.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_GetMultipleBlockProtectionStatus(phalICode_Sw_DataParams_t * pDataParams, uint8_t bBlockNo, uint8_t bNoOfBlocks,
        uint8_t * pProtectionStates, uint16_t * pNumReceivedStates)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_GetMultipleBlockProtectionStatus(
            pDataParams->pPalSli15693DataParams,
            pDataParams->bBuffering,
            bBlockNo,
            bNoOfBlocks,
            pProtectionStates,
            pNumReceivedStates));

    return PH_ERR_SUCCESS;
}

/*
 * Performs Destroy command. This command permanently destroys the label.
 *
 * The Destroy password has to be transmitted before with \ref phalICode_SetPassword.
 * Remark: This command is irreversible and the label will never respond to any command again.
 * Remark: This command can only be executed in addressed or selected mode.
 *
 * Note: This command is not valid for ICode Dna product as the Destroy feature is part of Mutual
 * Authentication command (refer \ref phalICode_AuthenticateMAM).
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      pXorPwd             : XOR Password; 32 bits. Pass the password for the ICODE products that supports and NULL
 *                            for the products that do not support.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_Destroy(phalICode_Sw_DataParams_t * pDataParams, uint8_t * pXorPwd)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_Destroy(pDataParams->pPalSli15693DataParams, pXorPwd));

    return PH_ERR_SUCCESS;
}

/*
 * Performs EnablePrivacy command. This command instructs the label to enter privacy mode.
 *
 * In privacy mode, the label will only respond to \ref phalSli_GetRandomNumber and \ref phalICode_SetPassword commands.
 * To get out of the privacy mode, the Privacy password has to be transmitted before with \ref phalICode_SetPassword.
 *
 * Note: This command is not valid for ICode Dna product as the Destroy feature is part of Mutual
 * Authentication command (refer \ref phalICode_AuthenticateMAM).
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      pXorPwd             : XOR Password; 32 bits. Pass the password for the ICODE products that supports and NULL
 *                            for the products that do not support.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_EnablePrivacy(phalICode_Sw_DataParams_t * pDataParams, uint8_t * pXorPwd)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_EnablePrivacy(
            pDataParams->pPalSli15693DataParams,
            pXorPwd));

    return PH_ERR_SUCCESS;
}

/*
 * Perform 64-BitPasswordProtection command. This instructs the label that both of the Read and Write passwords
 * are required for protected access.
 *
 * Note that both the Read and Write passwords have to be transmitted before with \ref phalICode_SetPassword.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_64BitPasswordProtection(phalICode_Sw_DataParams_t * pDataParams)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_64BitPasswordProtection(pDataParams->pPalSli15693DataParams));

    return PH_ERR_SUCCESS;
}

/*
 * Performs ReadSignature command. On this command, the label will respond with the signature value.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *
 * Output Parameters:
 *      ppSign              : The originality signature returned by the VICC.
 *      ppSign              : Length of originality signature buffer.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ReadSignature(phalICode_Sw_DataParams_t * pDataParams, uint8_t ** ppSign, uint16_t * pSignLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ReadSignature(pDataParams->pPalSli15693DataParams, ppSign, pSignLen));

    return PH_ERR_SUCCESS;
}

/*
 * Reads a multiple 4 byte(s) data from the mentioned configuration block address. Here the starting address of the
 * configuration block should be given in the parameter bBlockAddr and the number of blocks to read from the starting
 * block should be given in the parameter bNoOfBlocks.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      bBlockAddr          : Configuration block address.
 *      bNoOfBlocks         : The n block(s) to read the configuration data.
 *
 * Output Parameters:
 *      ppData              : Multiple of 4 (4u * No Of Blocks) byte(s) of data read from the mentioned
 *                            configuration block address.
 *      pDataLen            : Number of received configuration data bytes.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_ReadConfig(phalICode_Sw_DataParams_t * pDataParams, uint8_t bBlockAddr, uint8_t bNoOfBlocks, uint8_t ** ppData,
        uint16_t * pDataLen)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_ReadConfig(
            pDataParams->pPalSli15693DataParams,
            bBlockAddr,
            bNoOfBlocks,
            ppData,
            pDataLen));

    return PH_ERR_SUCCESS;
}

/*
 * Writes a 4 byte data to the mentioned configuration block address.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      bOption             : Options to be enabled or disabled. As per ISO15693 protocol
 *                              0x00:   PHAL_ICODE_OPTION_OFF Disable option.
 *                              0x01:   PHAL_ICODE_OPTION_ON Enable option.
 *      bBlockAddr          : Configuration block address.
 *      pData               : A 4 byte data to be written to the mentioned configuration block address.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_WriteConfig(phalICode_Sw_DataParams_t * pDataParams, uint8_t bOption, uint8_t bBlockAddr, uint8_t * pData)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_WriteConfig(
            pDataParams->pPalSli15693DataParams,
            bOption,
            bBlockAddr,
            pData));

    return PH_ERR_SUCCESS;
}

/*
 * Enables the random ID generation in the tag. This interfaces is used to instruct the tag to generate
 * a random number in privacy mode.
 *
 * Flag can be set by using \ref phalICode_SetConfig command
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_PickRandomID(phalICode_Sw_DataParams_t * pDataParams)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_PickRandomID(pDataParams->pPalSli15693DataParams));

    return PH_ERR_SUCCESS;
}


/*
 * Get the configuration settings.
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      wConfig             : Configuration to read.
 *                              0x00: PHAL_ICODE_CONFIG_FLAGS
 *                              0x01: PHAL_ICODE_CONFIG_ADD_INFO
 *                              0x02: PHAL_ICODE_CONFIG_TIMEOUT_US
 *                              0x03: PHAL_ICODE_CONFIG_TIMEOUT_MS
 *                              0x04: PHAL_ICODE_CONFIG_ENABLE_BUFFERING
 *
 * Output Parameters:
 *      pValue              : The value for the mentioned configuration information in wConfig parameter.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_GetConfig(phalICode_Sw_DataParams_t * pDataParams, uint16_t wConfig, uint16_t * pValue)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    /* Retrieve the configuration settings. */
    switch(wConfig)
    {
    case PHAL_ICODE_CONFIG_ENABLE_BUFFERING:
        *pValue = pDataParams->bBuffering;
        break;

    case PHAL_ICODE_CONFIG_FLAGS:
    case PHAL_ICODE_CONFIG_ADD_INFO:
    case PHAL_ICODE_CONFIG_TIMEOUT_US:
    case PHAL_ICODE_CONFIG_TIMEOUT_MS:
        PH_CHECK_SUCCESS_FCT(wStatus, phpalSli15693_GetConfig(
                pDataParams->pPalSli15693DataParams,
                wConfig,
                pValue));
        break;

    default:
        return PH_ADD_COMPCODE_FIXED(PH_ERR_INVALID_PARAMETER, PH_COMP_AL_ICODE);
    }

    return PH_ERR_SUCCESS;
}

/*
 * Set the configuration settings.
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *      wConfig             : Configuration to write.
 *                              0x00: PHAL_ICODE_CONFIG_FLAGS
 *                              0x04: PHAL_ICODE_CONFIG_ENABLE_BUFFERING
 *      wValue              : The value for the mentioned configuration information in wConfig parameter.
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_SetConfig(phalICode_Sw_DataParams_t * pDataParams, uint16_t wConfig, uint16_t wValue)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    /* Update the configuration settings. */
    switch(wConfig)
    {
    case PHAL_ICODE_CONFIG_ENABLE_BUFFERING:
        pDataParams->bBuffering = (uint8_t) wValue;
            PH_CHECK_SUCCESS_FCT(wStatus, phpalSli15693_SetConfig(
                pDataParams->pPalSli15693DataParams,
                PHPAL_SLI15693_CONFIG_ENABLE_BUFFERING,
                wValue));
        break;

    case PHAL_ICODE_CONFIG_FLAGS:
        PH_CHECK_SUCCESS_FCT(wStatus, phpalSli15693_SetConfig(
                pDataParams->pPalSli15693DataParams,
                wConfig,
                wValue));
        break;

    default:
        return PH_ADD_COMPCODE_FIXED(PH_ERR_INVALID_PARAMETER, PH_COMP_AL_ICODE);
    }

    return PH_ERR_SUCCESS;
}

/*
 * Get the type of Tag
 *
 * Input Parameters:
 *      pDataParams         : Pointer to this layer's parameter structure.
 *
 * Output Parameters:
 *      pTagType            : The type of ICode tag.
 *                              0xFFFF: PHAL_ICODE_TAG_TYPE_UNKNOWN
 *                              0x0001: PHAL_ICODE_TAG_TYPE_ICODE_SLI
 *                              0x0002: PHAL_ICODE_TAG_TYPE_ICODE_SLI_S
 *                              0x0003: PHAL_ICODE_TAG_TYPE_ICODE_SLI_L
 *                              0x5001: PHAL_ICODE_TAG_TYPE_ICODE_SLIX
 *                              0x5002: PHAL_ICODE_TAG_TYPE_ICODE_SLIX_S
 *                              0x5003: PHAL_ICODE_TAG_TYPE_ICODE_SLIX_L
 *                              0x0801: PHAL_ICODE_TAG_TYPE_ICODE_SLI_X2
 *                              0x1801: PHAL_ICODE_TAG_TYPE_ICODE_DNA
 *                              0x5801: PHAL_ICODE_TAG_TYPE_ICODE_NTAG5_I2C
 *
 * Return:
 *          PH_ERR_SUCCESS for successfull operation.
 *          Other Depending on implementation and underlaying component.
 */
phStatus_t phalICode_Sw_GetTagType(phalICode_Sw_DataParams_t * pDataParams, uint16_t * pTagType)
{
    phStatus_t  PH_MEMLOC_REM wStatus = 0;

    PH_CHECK_SUCCESS_FCT(wStatus, phalICode_Int_GetTagType(
            pDataParams->pPalSli15693DataParams,
            pTagType));

    return PH_ERR_SUCCESS;
}

#endif /* NXPBUILD__PHAL_ICODE_SW */
