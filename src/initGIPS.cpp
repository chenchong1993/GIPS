/*------------------------------------------------------------------------------
* initGIPS : 初始化GIPS服务所用到结构体。
*
*
* options : linux 平台 codeblocks
*
* version : 1.0 by 陈冲
* history : 2019/3/2
*-----------------------------------------------------------------------------*/
#include <iostream>

#include "rtklib.h"
#define MIN_INT_RESET   30000   /* mininum interval of reset command (ms) */

/* initialize solopt_t -------------------------------------------------------
* 初始化solopt_t结构体
* args   : solopt_t *SolOpt
* return : status (0:error,1:ok)
*-----------------------------------------------------------------------------*/
extern int rtksoloptinit(solopt_t *SolOpt)
{
    SolOpt->posf = 1;//1：XYZ,2:ENU
    SolOpt->times = 0;
    SolOpt->timef = 1;
    SolOpt->timeu = 3;
    SolOpt->degf = 0;
    SolOpt->outhead = 1;
    SolOpt->outopt = 1;
    SolOpt->outvel = 0;/* output velocity options (0:no,1:yes) */
    SolOpt->datum = 0;
    SolOpt->height = 0;
    SolOpt->geoid = 0;
    SolOpt->solstatic = 0;      /* solution of static mode (0:all,1:single) */
    SolOpt->sstat = 0;          /* solution statistics level (0:off,1:states,2:residuals) */
    SolOpt->trace = 0;          /* debug trace level (0:off,1-5:debug) */

    SolOpt->nmeaintv[0] = 0;
    SolOpt->nmeaintv[1] = 0;
    strcpy(SolOpt->sep, " ");
    strcpy(SolOpt->prog, " "); //
    SolOpt->maxsolstd = 0;   /* max std-dev for solution output (m) (0:all) */
    return 1;
}
/* initialize prcopt_t -------------------------------------------------------
* 初始化prcopt_t结构体
* args   : prcopt_t *PrcOpt
* return : status (0:error,1:ok)
*-----------------------------------------------------------------------------*/
extern int rtkprcoptinit(prcopt_t *PrcOpt,rtksvr_t *rtksvr)
{
    PrcOpt->mode = PMODE_SINGLE;
    //PrcOpt->soltype =
    PrcOpt->nf = 2;
    PrcOpt->navsys = SYS_GPS|SYS_GLO| SYS_CMP;//SYS_GPS | SYS_CMP |
    PrcOpt->elmin = 15*D2R;
    PrcOpt->snrmask.ena[0] = 0; PrcOpt->snrmask.ena[1] = 0;
    PrcOpt->sateph = EPHOPT_BRDC;
    PrcOpt->modear = ARMODE_CONT;
    PrcOpt->glomodear = 0;
    PrcOpt->bdsmodear = 0;
    //PrcOpt->maxout =
    //PrcOpt->minlock =
    PrcOpt->minfix = 30;
    //PrcOpt->armaxiter =
    PrcOpt->ionoopt = IONOOPT_BRDC;
    PrcOpt->tropopt = TROPOPT_SAAS;
    PrcOpt->dynamics = 0;
    PrcOpt->tidecorr = 0;
    PrcOpt->niter = 1;
    //PrcOpt->codesmooth =
    //PrcOpt->intpref
    //PrcOpt->sbascorr
    //PrcOpt->sbassatsel

    PrcOpt->eratio[0] = 100; PrcOpt->eratio[1] = 100;
    PrcOpt->err[1] = 0.003; PrcOpt->err[2] = 0.003; PrcOpt->err[3] = 0; PrcOpt->err[4] = 1;
     //PrcOpt->std
    PrcOpt->prn[0] = 0.0001; PrcOpt->prn[1] = 0.001; PrcOpt->prn[2] = 0.0001; PrcOpt->prn[3] = 10; PrcOpt->prn[4] = 10;
    PrcOpt->sclkstab = 5e-12;
    PrcOpt->thresar[0] = 3;
    PrcOpt->elmaskar = 25*D2R;
    PrcOpt->elmaskhold = 25*D2R;
    PrcOpt->thresslip = 0.05;
    PrcOpt->maxtdiff = 30;
    PrcOpt->maxinno = 30;
    PrcOpt->maxgdop = 30;
    //PrcOpt->anttype
    //PrcOpt->antdel
    PrcOpt->exsats[38 - 1] = 1;
    PrcOpt->maxaveep = 3600;
    PrcOpt->initrst = 1;
    //PrcOpt->outsingle
    //PrcOpt->rnxopt
    PrcOpt->posopt[0] = 0; PrcOpt->posopt[1] = 0; PrcOpt->posopt[2] = 0; PrcOpt->posopt[3] = 0;PrcOpt->posopt[4] = 1; PrcOpt->posopt[5] = 0;
    PrcOpt->syncsol = 0;
    //PrcOpt->odisp
    //PrcOpt->exterr
    //PrcOpt->freqopt
    //PrcOpt->pppopt

    int RovPosTypeF = 0; int RefPosTypeF = 2;int RovAntPcvF = 0;int RefAntPcvF = 0,BaselineC = 0,sat, ex;
    double RovAntDel[3], RefAntDel[3], RovPos[3], RefPos[3],Baseline[2];
    std::string InTimeStart, InTimeSpeed, ExSats;
	std::string RovAntF, RefAntF, SatPcvFileF, AntPcvFileF;
	char buff[1024], *p;
    pcvs_t pcvr, pcvs;
    pcv_t *pcv;
    gtime_t time = timeget();
    RovPos[0] = 3.932578216166497e-12;
    RovPos[1] = 0;
    RovPos[2] = 0;
    RefPos[0] = -2117077.681;//47
        //-2130200.653;//46
    RefPos[1] = 4584471.599;
        //4557740.534;//
    RefPos[2] = 3883288.971;
        //3907348.72;//

    Baseline[0] = 0; Baseline[1] = 0;
    memset(&pcvr, 0, sizeof(pcvs_t));
    memset(&pcvs, 0, sizeof(pcvs_t));

    if (RovPosTypeF <= 2) { // LLH,XYZ
        PrcOpt->rovpos = 0;
        PrcOpt->ru[0] = RovPos[0];
        PrcOpt->ru[1] = RovPos[1];
        PrcOpt->ru[2] = RovPos[2];
    }
    else { // RTCM position
        PrcOpt->rovpos = 4;
        for (int i = 0; i<3; i++) PrcOpt->ru[i] = 0.0;
    }

    if (RefPosTypeF <= 2) { // LLH,XYZ
        PrcOpt->refpos = 0;
        PrcOpt->rb[0] = RefPos[0];
        PrcOpt->rb[1] = RefPos[1];
        PrcOpt->rb[2] = RefPos[2];
    }
    else if (RefPosTypeF == 3) { // RTCM position
        PrcOpt->refpos = 4;
        for (int i = 0; i<3; i++) PrcOpt->rb[i] = 0.0;
    }
    else { // average of single position
        PrcOpt->refpos = 1;
        for (int i = 0; i<3; i++) PrcOpt->rb[i] = 0.0;
    }

    for (int i = 0; i<MAXSAT; i++) {
        PrcOpt->exsats[i] = 0;
    }
    if (ExSats != "") { // excluded satellites
        strcpy(buff, ExSats.c_str());
        for (p = strtok(buff, " "); p; p = strtok(NULL, " ")) {
            if (*p == '+') { ex = 2; p++; }
            else ex = 1;
            if (!(sat = satid2no(p))) continue;
            PrcOpt->exsats[sat - 1] = ex;
        }
    }
    if ((RovAntPcvF || RefAntPcvF) && !readpcv(AntPcvFileF.c_str(), &pcvr)) {
        trace(3, "rcv ant file read error %s", AntPcvFileF.c_str());
        return 0;
    }

    if (RovAntPcvF) {
        if ((pcv = searchpcv(0, RovAntF.c_str(), time, &pcvr))) {
            PrcOpt->pcvr[0] = *pcv;
        }
        else {
            trace(3, "no antenna pcv %s", RovAntF.c_str());
        }
        for (int i = 0; i<3; i++) PrcOpt->antdel[0][i] = RovAntDel[i];
    }

    if (RefAntPcvF) {
        if ((pcv = searchpcv(0, RefAntF.c_str(), time, &pcvr))) {
            PrcOpt->pcvr[1] = *pcv;
        }
        else {
            trace(3, "no antenna pcv %s", RefAntF.c_str());
        }
        for (int i = 0; i<3; i++) PrcOpt->antdel[1][i] = RefAntDel[i];
    }

    if (RovAntPcvF || RefAntPcvF) {
        free(pcvr.pcv);
    }

    if (PrcOpt->sateph == EPHOPT_PREC || PrcOpt->sateph == EPHOPT_SSRCOM) {
        if (!readpcv(SatPcvFileF.c_str(), &pcvs)) {
            trace(3, "sat ant file read error %s", SatPcvFileF.c_str());
            return 0;
        }
        for (int i = 0; i<MAXSAT; i++) {
            if (!(pcv = searchpcv(i + 1, "", time, &pcvs))) continue;
            rtksvr->nav.pcvs[i] = *pcv;
        }
        free(pcvs.pcv);
    }

    if (BaselineC) {
        PrcOpt->baseline[0] = Baseline[0];
        PrcOpt->baseline[1] = Baseline[1];
    }
    else {
        PrcOpt->baseline[0] = 0.0;
        PrcOpt->baseline[1] = 0.0;
    }



    return 1;
}
