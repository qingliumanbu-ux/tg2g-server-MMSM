/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    石咏
Version:    1.0
Date:       2016-05-24
Description: 方坯拆批
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 方坯拆批
/// <para>
/// * 按传入TMMSM01表的信息新增新材料号
/// * 修改母材料号的支数和理重,实重与理重相同
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  /*炼钢物料主表*/
 
 
#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数

#endif


//外部函数声明 
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_get_matno(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);    //生成拆批材料号
int f_mmsm_matno_catch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);    //生成拆批材料号
int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_get_theorywt(CDecimal MAT_ACT_THICK, CDecimal MAT_ACT_WIDTH, CDecimal MAT_ACT_LEN, CDecimal MAT_NUM, CDecimal &MAT_THEORY_WT);

#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数
int f_pmof99_v3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif


int f_mmsm81(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_mat_no_new("");
	CString	cs_mat_no_max("");
	CString	cs_mat_no_max_sub("");
	CDecimal cs_mat_no_max_len = 0;	//材料长度
	CDecimal cs_mat_no_max_id = 0;	//材料流水位
	CDecimal cd_mat_tube = 0;
	CString resumeSeqNo = "";
	CString tcBacklog = "";
	CString ymque = "";
	CString eventDesc = "";
	CString unPlanNo = "";
	CString unOrderNo = "";
	CString ifTrace = "";
	CString funcId = "";
	CString ifTcSnd = "";
	CDecimal v_old_mat_wt = 0;
	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";
	/* 实体类定义 */ 
	CModel tmmsm96("TMMSM96");
	CModel new_tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
	CModel new_tmmsm01("TMMSM01");
	CModel tmmsm35("TMMSM35");

	EIClass inBlock;          //材料主档信息处理用
	EIClass outBlock;          //材料主档信息处理用

	#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数
	CModel tpmof03("TPMOF03");
	#endif

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		CDbCommand getSeq("SELECT MMSM_MATNO_SEQ.NEXTVAL  FROM SYSIBM.SYSDUMMY1", conn);
		CString newSeqNo = "0000" + getSeq.ExecuteScalar().ToString().Trim();
		newSeqNo = newSeqNo.Substring(newSeqNo.GetLength() - 4);
		//Log::Trace("", "", "newSeqNo=[{0}]", newSeqNo);
		resumeSeqNo = datetime.Trim() + newSeqNo;
		//Log::Trace("", "", "resumeSeqNo=[{0}]", resumeSeqNo);

		tmmsm35["REC_CREATE_TIME"] = datetime;
		tmmsm35["REC_CREATOR"] = s.userid;
		tmmsm35["PROD_TIME"] = datetime;

		/*--------------------------------------------------------------班次班组计算---------------------------------------------------------------------*/
		#if defined _SYS_PES || defined _SYS_MES
		//Log::Trace("", "", "tmmsm35["PROD_TIME"] =[{0}]", tmmsm35["PROD_TIME"].ToString());
		//f_epep_get_shift_group("SM", tmmsm35["PROD_TIME"].ToString(), tmmsm35["PROD_SHIFT_NO"].ToString(), tmmsm35["PROD_SHIFT_GROUP"].ToString(), conn);
		//Log::Trace("", "", "tmmsm35["PROD_SHIFT_NO"] =[{0}]", tmmsm35["PROD_SHIFT_NO"].ToString());
		//Log::Trace("", "", "tmmsm35["PROD_SHIFT_GROUP"] =[{0}]", tmmsm35["PROD_SHIFT_GROUP"].ToString());
		f_epep_get_shift_group("SM", tmmsm35["PROD_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
		tmmsm35["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
		tmmsm35["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;
		/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
		#endif

		tmmsm35["PROD_MAKER"] = s.userid;
		tmmsm35["PROD_SEQ_NO"] = resumeSeqNo;

		blkNum = bcls_rec->Tables.IndexOf("PMOF99");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("PMOF99");
		}



		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("MMSM81");
		if (blkNum < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			strcpy(s.sysmsg, "传入数据块 MMSM81 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
		}

		/* 设置返回块名 */
		blkNum = bcls_ret->Tables.IndexOf("MMSM81");				//设置返回块名
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("MMSM81");
			bcls_ret->Tables["MMSM81"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_ret->Tables["MMSM81"].Columns.Add(DT_DECIMAL, "MAT_NUM");
			bcls_ret->Tables["MMSM81"].Columns.Add(DT_DECIMAL, "MAT_ACT_WT");
			bcls_ret->Tables["MMSM81"].Columns.Add(DT_STRING, "MAT_NO_NEW");
			bcls_ret->Tables["MMSM81"].Columns.Add(DT_DECIMAL, "MAT_NUM_NEW");
			bcls_ret->Tables["MMSM81"].Columns.Add(DT_DECIMAL, "MAT_ACT_WT_NEW");
			bcls_ret->Tables["MMSM81"].Rows.Add();
		}



		tmmsm96["EVENT_ID"] = bcls_rec->Tables["MMSM81"].Rows[0]["EVENT_ID"].ToString().Trim();	//调用事件号
		tmmsm96["EVENT_LINE_TYPE"] = bcls_rec->Tables["MMSM81"].Rows[0]["EVENT_LINE_TYPE"].ToString().Trim();	//调用事件号
		tmmsm96["MAT_NO"] = bcls_rec->Tables["MMSM81"].Rows[0]["MAT_NO"].ToString().Trim();	//母材料号
		tmmsm96["MAT_NUM"] = bcls_rec->Tables["MMSM81"].Rows[0]["MAT_NUM"].ToDecimal();
		if (bcls_rec->Tables["MMSM81"].Columns.Contains("MAT_NO_NEW"))
		{
			cs_mat_no_new = bcls_rec->Tables["MMSM81"].Rows[0]["MAT_NO_NEW"].ToString().Trim();//新材料号-可为空
		}
		if (bcls_rec->Tables["MMSM81"].Columns.Contains("TC_BACKLOG"))	//炼钢侧物料管理母材料拆批
		{
			tcBacklog = bcls_rec->Tables["MMSM81"].Rows[0]["TC_BACKLOG"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM81"].Columns.Contains("YMQUE"))                       //用于判断拆出的子坯是否需要建立入库队列
		{
			ymque = bcls_rec->Tables["MMSM81"].Rows[0]["YMQUE"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM81"].Columns.Contains("FUNC_ID"))
		{
			funcId = bcls_rec->Tables["MMSM81"].Rows[0]["FUNC_ID"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM81"].Columns.Contains("EVENT_DESC"))
		{
			eventDesc = bcls_rec->Tables["MMSM81"].Rows[0]["EVENT_DESC"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM81"].Columns.Contains("UNPLAN"))
		{
			unPlanNo = bcls_rec->Tables["MMSM81"].Rows[0]["UNPLAN"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM81"].Columns.Contains("UNORDER"))
		{
			unOrderNo = bcls_rec->Tables["MMSM81"].Rows[0]["UNORDER"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM81"].Columns.Contains("IF_TRACE"))
		{
			ifTrace = bcls_rec->Tables["MMSM81"].Rows[0]["IF_TRACE"].ToString().Trim();
		}

		ifTcSnd = "1";//默认需要调用发送电文（考虑其他模块调用）
		if (bcls_rec->Tables["MMSM81"].Columns.Contains("IF_TC_SND"))
		{
			ifTcSnd = bcls_rec->Tables["MMSM81"].Rows[0]["IF_TC_SND"].ToString().Trim();
		}



		/* 检查输入参数合法性 */
		if (tmmsm96["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, _RES("GCRSS0000035")/*母材料号不能为空。*/);
			strcpy(s.sysmsg, "数据校验失败，传入的母材料号不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm96["MAT_NUM"].ToDecimal() == 0)
		{
			sprintf(s.msg, _RES("MMSMS0000228")/*材料支数不能为0。*/);
			strcpy(s.sysmsg, "数据校验失败，材料支数不能为。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		tmmsm01["MAT_NO"] = tmmsm96["MAT_NO"];
		//Log::Trace("", __FUNCTION__, "tmmsm01.MAT_NO			= [{0}]", tmmsm01["MAT_NO"].ToString());
		if (!tmmsm01.Query())
		{
			sprintf(s.msg, "未找到母材料信息");
			strcpy(s.sysmsg, s.msg);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		new_tmmsm01.CopyFrom(tmmsm01);

		v_old_mat_wt = tmmsm01["MAT_ACT_WT"];

		if (tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() != "A")
		{
			CFormattable arguments[] = { (const char*)tmmsm01["MAT_NO"].ToString() };// 定义参数列表的数组
			CMessageFormat::Format(s.msg, "材料[{0}]形态必须为小方坯", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//Log::Trace("", __FUNCTION__, "tmmsm01.PLAN_NO			= [{0}]", tmmsm01["PLAN_NO"].ToString());
		if (unPlanNo == "1" && tmmsm01["PLAN_NO"].ToString().Trim() != "")
		{
			sprintf(s.msg, "材料有计划，不允许在此拆批");
			strcpy(s.sysmsg, "材料有计划，不允许在此拆批");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//Log::Trace("", __FUNCTION__, "tmmsm01.ORDER_NO			= [{0}]", tmmsm01["ORDER_NO"].ToString());
		if (unOrderNo == "1" && tmmsm01["ORDER_NO"].ToString().Trim() != "")
		{
			sprintf(s.msg, "材料有合同，不允许在此拆批");
			strcpy(s.sysmsg, "材料有合同，不允许在此拆批");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//Log::Trace("", __FUNCTION__, "tmmsm01.MAT_NUM			= [{0}]", tmmsm01["MAT_NUM"].ToDecimal());
		/* 校验是否能拆批 */
		if (tmmsm01["MAT_NUM"].ToDecimal() <= tmmsm96["MAT_NUM"].ToDecimal())
		{
			strcpy(s.sysmsg, "拆批支数不能大于等于原支数");
			strcpy(s.msg, "拆批支数不能大于等于原支数");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		#pragma region  处理新材料信息
		//Log::Trace("", "", "处理母材料信息开始");
		/* 指定拆批材料号有值-新材料号等于传入指定拆批材料号 */
		if (cs_mat_no_new.Trim() != "")
		{
			new_tmmsm01["MAT_NO"] = cs_mat_no_new;
		}
		else
		{
			if (!bcls_rec->Tables.Contains("GENMATNO"))
			{
				bcls_rec->Tables.Add("GENMATNO");
				bcls_rec->Tables["GENMATNO"].Columns.Add(DT_STRING, "MAT_NO");
				bcls_rec->Tables["GENMATNO"].Rows.Add();
			}
			bcls_rec->Tables["GENMATNO"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];


			//doFlag = f_mmsm_get_matno(bcls_rec, bcls_ret, conn);

			if (!inBlock.Tables[0].Columns.Contains("MAT_NO_OLD"))
			{
				inBlock.Tables[0].Columns.Add(DT_STRING, "MAT_NO_OLD");
			}
			if (!inBlock.Tables[0].Columns.Contains("FUNC_ID"))
			{
				inBlock.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
			}
			inBlock.Tables[0].Rows.Clear();
			inBlock.Tables[0].Rows.Add();
			inBlock.Tables[0].Rows[0]["MAT_NO_OLD"] = tmmsm01["MAT_NO"];
			inBlock.Tables[0].Rows[0]["FUNC_ID"] = "MM81_MAT_NO"; //此处在EPED54定制化
			doFlag = f_mmsm_matno_catch(&inBlock, &outBlock, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			new_tmmsm01["MAT_NO"] = outBlock.Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
			//new_tmmsm01["MAT_NO"] = bcls_rec->Tables["GENMATNO"].Rows[0]["NEW_MAT_NO"].ToString().Trim();
		}

		//Log::Trace("", __FUNCTION__, "insert new_tmmsm01["MAT_NO"] = [{0}] ", (const char*)new_tmmsm01["MAT_NO"].ToString());

		/* 按传入材料信息新增材料档 */
		new_tmmsm01["MAT_NO_OLD"] = tmmsm01["MAT_NO"];
		new_tmmsm01["MAT_NUM"] = tmmsm96["MAT_NUM"];
		new_tmmsm01["MAT_NUM_CUT"] = tmmsm96["MAT_NUM"];

		//new_tmmsm01.QTY = tmmsm96.MAT_TUBE;
		//new_tmmsm01["MAT_NUM"] = tmmsm96.MAT_TUBE;
		//new_tmmsm01.REMAIN_NUM = tmmsm96.MAT_TUBE;

		new_tmmsm01["MAT_ACT_WT"] = tmmsm01["MAT_ACT_WT"].ToDecimal() * new_tmmsm01["MAT_NUM"].ToDecimal() / tmmsm01["MAT_NUM"];
		new_tmmsm01["MAT_ACT_WT"] = new_tmmsm01["MAT_ACT_WT"].ToDecimal().Round(3);	//小数点后暂时按3位四舍五入
		CDecimal MAT_THEORY_WT = 0;
		doFlag = f_mmsm_get_theorywt(new_tmmsm01["MAT_ACT_THICK"].ToDecimal(), new_tmmsm01["MAT_ACT_WIDTH"].ToDecimal(), new_tmmsm01["MAT_ACT_LEN"].ToDecimal(), new_tmmsm01["MAT_NUM"].ToDecimal(), MAT_THEORY_WT);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		new_tmmsm01["MAT_THEORY_WT"] = MAT_THEORY_WT;
		//Log::Trace("", "", "理论重量new_tmmsm01["MAT_THEORY_WT"] = {0}", new_tmmsm01["MAT_THEORY_WT"].ToDecimal());
		//Log::Trace("",__FUNCTION__,"拆批材料的入库标记 new_tmmsm01.IN_FLAG		= [{0}]",new_tmmsm01["IN_FLAG"].ToString());
		if (new_tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "0") //0-未称重
		{
			new_tmmsm01["MAT_WT"] = new_tmmsm01["MAT_THEORY_WT"];
		}
		else if (new_tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "1") //1-已称重
		{
			new_tmmsm01["MAT_WT"] = new_tmmsm01["MAT_ACT_WT"];
		}
		new_tmmsm01["REC_CREATOR"] = s.userid;
		new_tmmsm01["REC_CREATE_TIME"] = datetime;
		new_tmmsm01["REC_REVISOR"] = " ";
		new_tmmsm01["REC_REVISE_TIME"] = " ";
		new_tmmsm01["SLABTOP_FLAG"] = "0";
		new_tmmsm01["SLABTOP_TIME"] = " ";
		new_tmmsm01.TrimOrBlank();
		//new_tmmsm01.Insert();

		/* 新材料记录履历 */
		new_tmmsm01["MAT_ID"] = "";
		new_tmmsm96.CopyFrom(new_tmmsm01);
		new_tmmsm96["EVENT_ID"] = "MM3A";	//材料为拆批产生
		new_tmmsm96["EVENT_LINE_TYPE"] = tmmsm96["EVENT_LINE_TYPE"];
		new_tmmsm96["SYSTEM_ID"] = tmmsm96["SYSTEM_ID"];
		if (funcId == "")
		{
			new_tmmsm96["EVENT_DESC"] = s.svc_name;
		}
		else
		{
			new_tmmsm96["EVENT_DESC"] = funcId;
		}
		new_tmmsm96["EVENT_DESC"] = eventDesc;
		new_tmmsm96.TrimOrBlank();
		bcls_rec->Tables["MM0099"].Clear();
		new_tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		#if defined _SYS_PES || defined _SYS_MES
		/*if (ymque == "1")
		{
		if (bcls_rec->Tables.Contains("YM00QUE") == false)
		{
		bcls_rec->Tables.Add("WM00QUE");
		}
		if (bcls_rec->Tables["WM00QUE"].Columns.Contains("MAT_NO") == false)
		{
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
		}
		if (bcls_rec->Tables["WM00QUE"].Columns.Contains("MAT_KIND") == false)
		{
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_KIND");
		}
		if (bcls_rec->Tables["WM00QUE"].Columns.Contains("MAT_LINE_TYPE") == false)
		{
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_LINE_TYPE");
		}
		if (bcls_rec->Tables["WM00QUE"].Columns.Contains("STOCK_OPER_ORDER") == false)
		{
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		}

		if (bcls_rec->Tables["WM00QUE"].Rows.get_Count() <= 0)
		{
		bcls_rec->Tables["WM00QUE"].Rows.Add();
		}
		bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NO"] = new_tmmsm01["MAT_NO"];
		bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_KIND"] = "SM";
		bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_LINE_TYPE"] = new_tmmsm01["MAT_LINE_TYPE"];
		bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = "1K";

		doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
		throw CApplicationException(-1, s.msg, log.Location);
		}
		}*/
		#endif

		if (ifTrace == "1")
		{
			tmmsm35["IN_MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm35["IN_HEAT_NO"] = tmmsm01["HEAT_NO"];
			tmmsm35["IN_PONO"] = tmmsm01["PONO"];
			tmmsm35["IN_SG_SIGN"] = tmmsm01["SG_SIGN"];
			tmmsm35["IN_ST_NO"] = tmmsm01["ST_NO"];
			tmmsm35["IN_MAT_THICK"] = tmmsm01["MAT_THICK"];
			tmmsm35["IN_MAT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm35["IN_MAT_LEN"] = tmmsm01["MAT_LEN"];
			tmmsm35["IN_MAT_WT"] = tmmsm01["MAT_ACT_WT"];
			tmmsm35["IN_MAT_TUBE"] = tmmsm01["MAT_NUM"];
			tmmsm35["MAT_NO"] = new_tmmsm01["MAT_NO"];
			tmmsm35["HEAT_NO"] = new_tmmsm01["HEAT_NO"];
			tmmsm35["PONO"] = new_tmmsm01["PONO"];
			tmmsm35["SG_SIGN"] = new_tmmsm01["SG_SIGN"];
			tmmsm35["ST_NO"] = new_tmmsm01["ST_NO"];
			tmmsm35["MAT_THICK"] = new_tmmsm01["MAT_THICK"];
			tmmsm35["MAT_WIDTH"] = new_tmmsm01["MAT_WIDTH"];
			tmmsm35["MAT_LEN"] = new_tmmsm01["MAT_LEN"];
			tmmsm35["MAT_WT"] = new_tmmsm01["MAT_ACT_WT"];
			tmmsm35["MAT_TUBE"] = new_tmmsm01["MAT_NUM"];
			tmmsm35["OP_DIV"] = "2";
			tmmsm35.Insert();
		}
		#pragma endregion

		#pragma region  处理母材料信息
		tmmsm01["MAT_NUM"] = tmmsm01["MAT_NUM"].ToDecimal() - new_tmmsm01["MAT_NUM"];  //拆后材料支数
		//tmmsm01.QTY = tmmsm01.QTY - new_tmmsm01.MAT_TUBE;		  //拆后材料QTY
		tmmsm01["MAT_ACT_WT"] = tmmsm01["MAT_ACT_WT"].ToDecimal() - new_tmmsm01["MAT_ACT_WT"];
		tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"].ToDecimal() - new_tmmsm01["MAT_THEORY_WT"];
		//tmmsm01["REC_REVISOR"] = s.userid;
		//tmmsm01["REC_REVISE_TIME"] = datetime;
		//tmmsm01.Update("MAT_NUM,"
		//	"MAT_ACT_WT,"
		//	"MAT_THEORY_WT,"
		//	"REC_REVISOR,"
		//	"REC_REVISE_TIME", "MAT_NO");

		//Log::Trace("", __FUNCTION__, "tmmsm96.EVENT_LINE_TYPE111111111			= [{0}]", tmmsm96["EVENT_LINE_TYPE"].ToString());

		/* 母材料记录履历 设置物料跟踪参数 */
		bcls_rec->Tables["MM0099"].Clear();
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_DESC");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NUM");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_ACT_WT");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_THEORY_WT");
		bcls_rec->Tables["MM0099"].Rows.Add();
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = tmmsm96["EVENT_ID"];
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = tmmsm96["EVENT_LINE_TYPE"];
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = tmmsm96["SYSTEM_ID"];
		if (funcId == "")
		{
			bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
		}
		else
		{
			bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = funcId;
		}
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = eventDesc;
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NUM"] = tmmsm01["MAT_NUM"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WT"] = tmmsm01["MAT_ACT_WT"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"];
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//记录拆并批履历
		if (ifTrace == "1")
		{
			tmmsm35["MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm35["HEAT_NO"] = tmmsm01["HEAT_NO"];
			tmmsm35["PONO"] = tmmsm01["PONO"];
			tmmsm35["SG_SIGN"] = tmmsm01["SG_SIGN"];
			tmmsm35["ST_NO"] = tmmsm01["ST_NO"];
			tmmsm35["MAT_THICK"] = tmmsm01["MAT_THICK"];
			tmmsm35["MAT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm35["MAT_LEN"] = tmmsm01["MAT_LEN"];
			tmmsm35["MAT_WT"] = tmmsm01["MAT_ACT_WT"];
			tmmsm35["MAT_TUBE"] = tmmsm01["MAT_NUM"];
			tmmsm35.Insert();
		}


		#if defined _SYS_MMS || defined _SYS_MES
		if (tmmsm01["ORDER_NO"].ToString().Trim().GetLength() > 0)
		{
			/* 母材料总量修正 */
			tpmof03["EVENT_ID"] = "54";
			tpmof03["SYSTEM_ID"] = "MMSM";
			tpmof03["MAT_STATUS"] = tmmsm01["MAT_STATUS"];
			tpmof03["FUNC_ID"] = "f_mmsm81";
			tpmof03["ORDER_NO"] = tmmsm01["ORDER_NO"];
			tpmof03["WHOLE_BACKLOG_NO"] = tmmsm01["WHOLE_BACKLOG_NO"];
			tpmof03["WHOLE_BACKLOG"] = tmmsm01["WHOLE_BACKLOG"];
			tpmof03["WHOLE_BACKLOG_SEQ"] = tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"];
			tpmof03["WHOLE_BACKLOG_CODE"] = tmmsm01["NEXT_WHOLE_BACKLOG_CODE"];
			tpmof03["PREV_MAT_STATUS"] = tmmsm01["MAT_STATUS"];
			if (tmmsm01["PLAN_NO"].ToString().Trim().GetLength() > 0)
			{
				tpmof03["IF_PLAN"] = "1";
			}
			else
			{
				tpmof03["IF_PLAN"] = "0";
			}
			tpmof03["MAT_NO"] = tmmsm01["MAT_NO"];
			tpmof03["PREV_WT"] = v_old_mat_wt;
			tpmof03["WT"] = tmmsm01["MAT_ACT_WT"];
			tpmof03["PREV_NUM"] = 1;
			tpmof03["NUM"] = 1;
			tpmof03.MergeTo(bcls_rec->Tables["PMOF99"], false);

			//拆批材料
			tpmof03["EVENT_ID"] = "5I";
			tpmof03["SYSTEM_ID"] = "MMSM";
			tpmof03["MAT_STATUS"] = new_tmmsm01["MAT_STATUS"];
			tpmof03["FUNC_ID"] = "f_mmsm81";
			tpmof03["ORDER_NO"] = new_tmmsm01["ORDER_NO"];
			tpmof03["WHOLE_BACKLOG_NO"] = new_tmmsm01["WHOLE_BACKLOG_NO"];
			tpmof03["WHOLE_BACKLOG"] = new_tmmsm01["WHOLE_BACKLOG"];
			tpmof03["WHOLE_BACKLOG_SEQ"] = new_tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"];
			tpmof03["WHOLE_BACKLOG_CODE"] = new_tmmsm01["NEXT_WHOLE_BACKLOG_CODE"];
			tpmof03["PREV_MAT_STATUS"] = tmmsm01["MAT_STATUS"];
			tpmof03["MAT_NO"] = new_tmmsm01["MAT_NO"];
			tpmof03["WT"] = new_tmmsm01["MAT_ACT_WT"];
			tpmof03["NUM"] = 1;
			tpmof03["PREV_WT"] = 0;
			tpmof03["PREV_NUM"] = 0;
			if (new_tmmsm01["PLAN_NO"].ToString().Trim().GetLength() > 0)
			{
				tpmof03["IF_PLAN"] = "1";
			}
			else
			{
				tpmof03["IF_PLAN"] = "0";
			}
			tpmof03.MergeTo(bcls_rec->Tables["PMOF99"], false);

			doFlag = f_pmof99_v3(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

		}
		#endif  

		#pragma endregion

	/*	#pragma region 电文处理
		//Log::Trace("", "", "ifTcSnd = {0},tcBacklog = {1}", ifTcSnd, tcBacklog);
		if (ifTcSnd == "1")
		{
			if (tcBacklog == "")
			{
				if (tmmsm01["SYS_CODE"].ToString() == "M1")
				{
					tcBacklog = "";
				}
				else
				{
					if (tmmsm01["SYS_CODE"].ToString() == "PA")
					{
						tcBacklog = "MMSM3G";
					}
					else if (tmmsm01["MAT_LINE_TYPE"].ToString() == "PD")
					{
						tcBacklog = "M1PDM4";
					}
				}
			}
			//Log::Trace("", "", "重置tcBacklog = {0}", tcBacklog);

			if (tcBacklog != "")
			{
				blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
				if (blkNum < 0)
				{
					bcls_rec->Tables.Add("MMSMSND");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("EVENT_ID"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "EVENT_ID");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("REC_CREATOR"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "REC_CREATOR");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("REC_CREATE_TIME"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "REC_CREATE_TIME");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("AIM_MAT_NO"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "AIM_MAT_NO");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("MAT_NO"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAT_NO");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("MAT_TUBE"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_DECIMAL, "MAT_TUBE");
				}
				bcls_rec->Tables["MMSMSND"].Rows.Add();
				bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = tcBacklog;
				bcls_rec->Tables["MMSMSND"].Rows[0]["EVENT_ID"] = bcls_rec->Tables["MMSM81"].Rows[0]["EVENT_ID"];
				bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATOR"] = s.userid;
				bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATE_TIME"] = datetime;
				bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_MAT_NO"] = tmmsm01["MAT_NO"];
				bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"] = new_tmmsm01["MAT_NO"];
				bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_TUBE"] = new_tmmsm01.MAT_TUBE;
				doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);

				if (doFlag < 0)
				{
					throw CApplicationException(doFlag, s.msg, log.Location);
				}
				//Log::Trace("", __FUNCTION__, "成功调用发送拆批电文：tmmsm96["MAT_NO"] = [%s] ", (const char*)tmmsm96["MAT_NO"].ToString());
			}
		}
		#pragma endregion*/

		/* 设置返回值 */
		bcls_ret->Tables["MMSM81"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];		//母材料号
		bcls_ret->Tables["MMSM81"].Rows[0]["MAT_NUM"] = tmmsm01["MAT_NUM"];
		bcls_ret->Tables["MMSM81"].Rows[0]["MAT_ACT_WT"] = tmmsm01["MAT_ACT_WT"];
		bcls_ret->Tables["MMSM81"].Rows[0]["MAT_NO_NEW"] = new_tmmsm01["MAT_NO"]; 	//新材料号
		bcls_ret->Tables["MMSM81"].Rows[0]["MAT_NUM_NEW"] = new_tmmsm01["MAT_NUM"];
		bcls_ret->Tables["MMSM81"].Rows[0]["MAT_ACT_WT_NEW"] = new_tmmsm01["MAT_ACT_WT"];

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}
