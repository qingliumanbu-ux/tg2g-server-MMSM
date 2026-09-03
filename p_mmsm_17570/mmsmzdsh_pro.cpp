/************************/
/*** 2023-11-13 ********/
/****   mfj **************/
/**** 收货确认 ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsmacsh_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsmsm_stock_in(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//调用仓库接口，进行板坯入库 
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
// service入口
BM2F_ENTERACE(mmsmzdsh_pro)
BM2_FUNCTION_EXPORT
int f_mmsmzdsh_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;
	CString v_update = "";
	CString s_formname = "";
	CString i_func_id = "";
	CString i_func = "";
	CString v_fields_str = "";
	CDecimal v_celm_act = 0;//碳元素
	CString v_shll_seq = "";//收货履历序号
	int blkNum = 0;
	CString vstrand_no = "";//流号
	//收货开关，1 为开   0为关闭   人工维护
	CString sh_switch_z = "";
	CString sh_switch_a = "";
	CString sh_switch_b = "";
	CString sh_switch_c = "";
	CString sh_switch_d = "";
	CString sh_switch_e = "";
	CString sh_switch_f = "";
	// 间隔时间   产出后间隔此时间后才能自动收货   人工维护该时间(MMSM33ZDSH表  PROC_TIME 字段)  mfj  20240102
	CDecimal proc_time_z = 0;
	CDecimal proc_time_a = 0;
	CDecimal proc_time_b = 0;
	CDecimal proc_time_c = 0;
	CDecimal proc_time_d = 0;
	CDecimal proc_time_e = 0;
	CDecimal proc_time_f = 0;
	//小于该时间点的可以进行收货确认   mfj   20240102
	CString prod_time_z = "";
	CString prod_time_a = "";
	CString prod_time_b = "";
	CString prod_time_c = "";
	CString prod_time_d = "";
	CString prod_time_e = "";
	CString prod_time_f = "";

	CString mesage_error = "";//不处理原因

	int mm0099_count = 0;
	int mmsmacsh_count = 0;
	int wm_stock_count = 0;

	CString v_resume_seq_no = "";//序号

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm33("TMMSM33");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm33shll("TMMSM33SHLL");//收货履历表，原始记录，每次收货新增进表后 不再更改改该数据  主键:材料号，序号
	CModel twma0 = CModel("TWMA0");
	CModel twma2 = CModel("TWMA2");
	CModel tmmsm3e("TMMSM3E");

	CDbCommand cmd_inq(conn);

	try
	{
		//return 0;//暂不处理 等初工允许后放开   mfj  20240507    
		//获取限制的间隔时间和开关标记 
		sqlstr = "SELECT STRAND_NO,SH_SWITCH,PROC_TIME FROM TMMSM33ZDSH";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			vstrand_no = cmd_inq.GetString(1);
			if (vstrand_no == "Z")
			{
				sh_switch_z = cmd_inq.GetString(2);
				proc_time_z = cmd_inq.GetDecimal(3);
			}
			else if (vstrand_no == "A")
			{
				sh_switch_a = cmd_inq.GetString(2);
				proc_time_a = cmd_inq.GetDecimal(3);
			}
			else if (vstrand_no == "B")
			{
				sh_switch_b = cmd_inq.GetString(2);
				proc_time_b = cmd_inq.GetDecimal(3);
			}
			else if (vstrand_no == "C")
			{
				sh_switch_c = cmd_inq.GetString(2);
				proc_time_c = cmd_inq.GetDecimal(3);
			}
			else if (vstrand_no == "D")
			{
				sh_switch_d = cmd_inq.GetString(2);
				proc_time_d = cmd_inq.GetDecimal(3);
			}
			else if (vstrand_no == "E")
			{
				sh_switch_e = cmd_inq.GetString(2);
				proc_time_e = cmd_inq.GetDecimal(3);
			}
			else if (vstrand_no == "F")
			{
				sh_switch_f = cmd_inq.GetString(2);
				proc_time_f = cmd_inq.GetDecimal(3);
			}
		}
		cmd_inq.Close();


		prod_time_z = CDateTime::Parse(datetime).AddSeconds(-proc_time_z.ToDouble()).ToString("yyyyMMddHHmmss");
		prod_time_a = CDateTime::Parse(datetime).AddSeconds(-proc_time_a.ToDouble()).ToString("yyyyMMddHHmmss");
		prod_time_b = CDateTime::Parse(datetime).AddSeconds(-proc_time_b.ToDouble()).ToString("yyyyMMddHHmmss");
		prod_time_c = CDateTime::Parse(datetime).AddSeconds(-proc_time_c.ToDouble()).ToString("yyyyMMddHHmmss");
		prod_time_d = CDateTime::Parse(datetime).AddSeconds(-proc_time_d.ToDouble()).ToString("yyyyMMddHHmmss");
		prod_time_e = CDateTime::Parse(datetime).AddSeconds(-proc_time_e.ToDouble()).ToString("yyyyMMddHHmmss");
		prod_time_f = CDateTime::Parse(datetime).AddSeconds(-proc_time_f.ToDouble()).ToString("yyyyMMddHHmmss");

		Log::Trace("", __FUNCTION__, "sh_switch_c0=[{0}] [{1}]  [{2}] ", sh_switch_z, proc_time_z, prod_time_z);
		Log::Trace("", __FUNCTION__, "sh_switch_c1=[{0}] [{1}]  [{2}] ", sh_switch_a, proc_time_a, prod_time_a);
		Log::Trace("", __FUNCTION__, "sh_switch_c2=[{0}] [{1}]  [{2}] ", sh_switch_b, proc_time_b, prod_time_b);
		Log::Trace("", __FUNCTION__, "sh_switch_c3=[{0}] [{1}]  [{2}] ", sh_switch_c, proc_time_c, prod_time_c);
		Log::Trace("", __FUNCTION__, "sh_switch_c4=[{0}] [{1}]  [{2}] ", sh_switch_d, proc_time_d, prod_time_d);
		Log::Trace("", __FUNCTION__, "sh_switch_c4=[{0}] [{1}]  [{2}] ", sh_switch_e, proc_time_e, prod_time_e);
		Log::Trace("", __FUNCTION__, "sh_switch_c4=[{0}] [{1}]  [{2}] ", sh_switch_f, proc_time_f, prod_time_f);



		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}

		blkNum = bcls_rec->Tables.IndexOf("TMMSM01");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("TMMSM01");
			bcls_rec->Tables["TMMSM01"].Columns.Add(tmmsm01);
		}

		blkNum = bcls_rec->Tables.IndexOf("MMSMACSH");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSMACSH");
			bcls_rec->Tables["MMSMACSH"].Columns.Add(tmmsm96);
		}
		bcls_rec->Tables["MMSMACSH"].Columns.Add(DT_STRING, "DEAL_FLAG");

		/**  调用仓库接口  进行入库操作    mfj  2023.12.19  太钢定制 **/
		/*置板坯命令产出标志*/
		blkNum = bcls_rec->Tables.IndexOf("WM_STOCK");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("WM_STOCK");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(twma0);
			bcls_rec->Tables["WM_STOCK"].Columns.Add(twma2);
			bcls_rec->Tables["WM_STOCK"].Rows.Clear();
		}

		//查询不处于收货状态，并且没有收货取消的    mfj  20240309
		//  收货标记为F 和E 的不自动收货  MFJ  20240424
		sqlstr = "select   CASE WHEN (SELECT PONO FROM TPSSM11 S WHERE S.PONO = T.PONO )  IS NULL THEN "
				"	'8' WHEN T.PONO_SLAB <> ' ' THEN '9' ELSE '3' END AS SLAB_TYPE_OLD,T.* from TMMSM01 T "
				" WHERE RCV_MAT_FLAG <> 'W' and RCV_MAT_FLAG <> 'S'  AND RCV_NO_STATUS <> 'S'"
				" AND RCV_MAT_FLAG <> 'F' AND RCV_MAT_FLAG <> 'E' AND MAT_WT>12 ";

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_rec->Tables["TMMSM01"], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//return doFlag;


		for (int i = 0; i < bcls_rec->Tables["TMMSM01"].Rows.get_Count(); i++)
		{
			//将结构体清空  数据清零
			tmmsm01.Reset();
			tmmsm96.Reset();
			tmmsm33shll.Reset();
			v_celm_act = 0;

			tmmsm01.MergeFrom(bcls_rec->Tables["TMMSM01"].Rows[i]);

			if (bcls_rec->Tables["TMMSM01"].Rows[i]["SLAB_TYPE_OLD"].ToString().Trim() == "8"&&
				tmmsm01["LSLAB_NO"].ToString() != "")
			{
				//当前计划已关闭，且未脱虚拟板坯，不允许收货!
				continue;
			}



			if (!tmmsm01.QueryCount("MAT_NO"))//TMMSM01表未获取数据
			{
				Log::Trace("", __FUNCTION__, "在线档未查到 MAT_NO[{0}] 数据 ", tmmsm01["MAT_NO"].ToString());
				continue;
				/*	strcpy(s.sysmsg, "在线档中不存在数据，请确认数据是否已归档!");
					strcpy(s.msg, s.sysmsg);
					throw CApplicationException(-1, s.msg, log.Location);*/
			}

			//将L4层的判断添加在这里
			if (!(tmmsm01["MAT_STATUS"].ToString() == "20" || tmmsm01["MAT_STATUS"].ToString() == "29" || tmmsm01["MAT_STATUS"].ToString() == "30" || tmmsm01["MAT_STATUS"].ToString() == "39" || (tmmsm01["MAT_STATUS"].ToString() == "23" && tmmsm01["COMPLEX_DECIDE_CODE"].ToString() == "0")))
			{
				Log::Debug("", __FUNCTION__, "材料状态[{0}]，不符合收货条件。", tmmsm01["MAT_STATUS"].ToString());
				continue;
				/*sprintf(s.sysmsg, "材料[%s]状态，不符合收货条件！！", (const char*)tmmsm01["MAT_NO"].ToString());
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);*/
			}

			//头尾坯不进行自动收货
			if (tmmsm01["MAT_NO"].ToString().Substring(8, 2) == "00" || tmmsm01["MAT_NO"].ToString().Substring(8, 2) == "99" || tmmsm01["MAT_NO"].ToString().Substring(8, 2) == "AA" || tmmsm01["MAT_NO"].ToString().Substring(8, 2) == "ZZ")
			{
				continue;
			}


			if (tmmsm01["STRAND_NO"].ToString() == "Z")
			{
				if (sh_switch_z != "1")//开关状态为关闭  跳过该条记录
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 开关未开Z", tmmsm01["MAT_NO"].ToString());
					continue;
				}
				if (tmmsm01["SLAB_CUT_TIME"].ToString() > prod_time_z)//如果切断时刻大于可允许收货时刻，此时不允许收货  跳过
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 时间小于可允许收货时间Z", tmmsm01["MAT_NO"].ToString());
					continue;
				}
			}
			else if (tmmsm01["STRAND_NO"].ToString() == "A")
			{
				if (sh_switch_a != "1")//开关状态为关闭  跳过该条记录
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 开关未开A", tmmsm01["MAT_NO"].ToString());
					continue;
				}
				if (tmmsm01["SLAB_CUT_TIME"].ToString() > prod_time_a)//如果切断时刻大于可允许收货时刻，此时不允许收货  跳过
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 时间小于可允许收货时间A", tmmsm01["MAT_NO"].ToString());
					continue;
				}
			}
			else if (tmmsm01["STRAND_NO"].ToString() == "B")
			{
				if (sh_switch_b != "1")//开关状态为关闭  跳过该条记录
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 开关未开B", tmmsm01["MAT_NO"].ToString());
					continue;
				}
				if (tmmsm01["SLAB_CUT_TIME"].ToString() > prod_time_b)//如果切断时刻大于可允许收货时刻，此时不允许收货  跳过
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 时间小于可允许收货时间B", tmmsm01["MAT_NO"].ToString());
					continue;
				}
			}
			else if (tmmsm01["STRAND_NO"].ToString() == "C")
			{
				if (sh_switch_c != "1")//开关状态为关闭  跳过该条记录
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 开关未开C", tmmsm01["MAT_NO"].ToString());
					continue;
				}
				if (tmmsm01["SLAB_CUT_TIME"].ToString() > prod_time_c)//如果切断时刻大于可允许收货时刻，此时不允许收货  跳过
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 时间小于可允许收货时间C", tmmsm01["MAT_NO"].ToString());
					continue;
				}
			}
			else if (tmmsm01["STRAND_NO"].ToString() == "D")
			{
				if (sh_switch_d != "1")//开关状态为关闭  跳过该条记录
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 开关未开D", tmmsm01["MAT_NO"].ToString());
					continue;
				}
				if (tmmsm01["SLAB_CUT_TIME"].ToString() > prod_time_d)//如果切断时刻大于可允许收货时刻，此时不允许收货  跳过
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 时间小于可允许收货时间D", tmmsm01["MAT_NO"].ToString());
					continue;
				}
			}
			else if (tmmsm01["STRAND_NO"].ToString() == "E")
			{
				if (sh_switch_e != "1")//开关状态为关闭  跳过该条记录
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 开关未开E", tmmsm01["MAT_NO"].ToString());
					continue;
				}
				if (tmmsm01["SLAB_CUT_TIME"].ToString() > prod_time_e)//如果切断时刻大于可允许收货时刻，此时不允许收货  跳过
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 时间小于可允许收货时间E", tmmsm01["MAT_NO"].ToString());
					continue;
				}
			}
			else if (tmmsm01["STRAND_NO"].ToString() == "F")
			{
				if (sh_switch_f != "1")//开关状态为关闭  跳过该条记录
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 开关未开F", tmmsm01["MAT_NO"].ToString());
					continue;
				}
				if (tmmsm01["SLAB_CUT_TIME"].ToString() > prod_time_f)//如果切断时刻大于可允许收货时刻，此时不允许收货  跳过
				{
					Log::Trace("", __FUNCTION__, " MAT_NO[{0}] 数据 时间小于可允许收货时间F", tmmsm01["MAT_NO"].ToString());
					continue;
				}
			}

			//头尾坯不自动收货   mfj   20240106  头尾坯根据材料号判断
			/*if (tmmsm01["SLAB_PLACE_CODE"].ToString().Trim() == "B" || tmmsm01["SLAB_PLACE_CODE"].ToString().Trim() == "T")
			{
				continue;
			}*/

			//当命令板坯号为空时，不自动收货  mfj   20240115   待确认
			if (tmmsm01["PONO_SLAB"].ToString().Trim() == "")
			{
				continue;
			}


			tmmsm33.MergeFrom(bcls_rec->Tables["TMMSM01"].Rows[i]);

			//单判碳元素
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr = "select ELM_ACT FROM TQMTS29 WHERE HEAT_NO = '" + tmmsm01["HEAT_NO"].ToString().Trim() + "' AND ELM_NAME = 'C'";
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				v_celm_act = cmd_inq.GetDecimal(1);
			}
			cmd_inq.Close();

			Log::Trace("", __FUNCTION__, "v_celm_act[{0}]  ", v_celm_act);

			//若没有碳元素值，则不允许收货  跳过该循环，进入下一循环
			if (v_celm_act == 0)
			{
				continue;
				/*strcpy(s.sysmsg, "碳元素没有值，请确认后再进行收货!");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);*/
			}

			//收货重量自动默认为名义重量  
			//改为取实时重量(若有称重量则为称重量，否则为三级理论量)  MFJ  20240309
			/*if (tmmsm01["RECEIVE_WEIGHT"].ToDecimal() == 0 || tmmsm01["RECEIVE_WEIGHT"].ToDecimal() < 0)
			{
				tmmsm01["RECEIVE_WEIGHT"] = tmmsm01["REAL_TIME_WT"];
			}*/
			//改为取实时重量(若有称重量则为称重量，否则为三级理论量)  MFJ  20240309
			if (tmmsm01["RECEIVE_WEIGHT"].ToDecimal() == 0 || tmmsm01["RECEIVE_WEIGHT"].ToDecimal() < 0)
			{
				tmmsm01["RECEIVE_WEIGHT"] = tmmsm01["MEASURE_WT"];
				if (tmmsm01["RECEIVE_WEIGHT"].ToDecimal() == 0 || tmmsm01["RECEIVE_WEIGHT"].ToDecimal() < 0)
				{
					tmmsm01["RECEIVE_WEIGHT"] = tmmsm01["MAT_WT"];
				}

				/*strcpy(s.sysmsg, "收获重量不允许小于等于0，请重新确认!");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);*/
			}

			tmmsm01["RECV_MAT_TIME"] = datetime;
			tmmsm01["AUTOSHOUHUO"] = "1";
			tmmsm01["AUTOSHOUHUOTIME"] = datetime;

			/*tmmsm33["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm33["RCV_MAT_FLAG"] = "W";//N 未收货  W等待(等L4的反馈)  S收货成功
			tmmsm33["RECV_MAT_TIME"] = tmmsm01["RECV_MAT_TIME"];

			tmmsm33.Update("RECEIVE_WEIGHT,RCV_MAT_FLAG,RECV_MAT_TIME", "MAT_NO");*/

			doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm3e.CopyFrom(tmmsm01);
			tmmsm3e["PROD_TIME"] = datetime;
			tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
			tmmsm3e["REMARK"] = "收货等待反馈";
			tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
			tmmsm3e.Insert();

			//RECEIVE_WEIGHT,RCV_MAT_FLAG,RECV_MAT_TIME

			
			/*tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
			//tmmsm96["RECEIVE_WEIGHT"] = tmmsm01["MAT_WT"];
			tmmsm96["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"];//收货重量，需要跟客户再确认一下是否其他重量字段一起更新。
			tmmsm96["MAT_WT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm96["MAT_ACT_WT"] = tmmsm01["RECEIVE_WEIGHT"];//等收货时有数据，与产销保持一致
			//tmmsm96["REAL_TIME_WT"] = tmmsm01["MAT_WT"]; //实时重量   
			Log::Trace("", __FUNCTION__, "RECEIVE_WEIGHT[{0}]  ", tmmsm01["RECEIVE_WEIGHT"].ToString());

			//名义规格与实际规格保持一致
			tmmsm96["MAT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm96["MAT_LEN"] = tmmsm01["MAT_LEN"];
			tmmsm96["MAT_THICK"] = tmmsm01["MAT_THICK"];

			//画面修改名义，用名义覆盖实际 保持两套一致
			tmmsm96["MAT_ACT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm96["MAT_ACT_LEN"] = tmmsm01["MAT_LEN"];
			tmmsm96["MAT_ACT_THICK"] = tmmsm01["MAT_THICK"];

			tmmsm96["SLAB_HEAD_WIDTH"] = tmmsm01["MAT_WIDTH"];//头宽
			tmmsm96["SLAB_TAIL_WIDTH"] = tmmsm01["MAT_WIDTH"];//尾宽

			tmmsm96["RECV_MAT_TIME"] = tmmsm01["RECV_MAT_TIME"];//收货日期
			tmmsm96["PRODUCT_FLAG"] = tmmsm01["PRODUCT_FLAG"];//成品标记
			tmmsm96["QUALIFIED_WT"] = tmmsm01["RECEIVE_WEIGHT"];//合格产量
			//tmmsm96["ACCEP_STLOC"] = "6242";
			tmmsm96["RCV_MAT_FLAG"] = "W";////N 未收货  W等待(等L4的反馈)  S收货成功
			tmmsm96["RCV_NO_STATUS"] = "N"; // 收货取消状态  置为 未取消  mfj  20240309
			tmmsm96["EVENT_ID"] = "MM34";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "mmsmacshf4_pro";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "铸坯收货确认";*/

			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM3H";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "mmsmzdsh_pro";
			tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm96["AUTOSHOUHUO"] = tmmsm01["AUTOSHOUHUO"];
			tmmsm96["AUTOSHOUHUOTIME"] = tmmsm01["AUTOSHOUHUOTIME"];
			tmmsm96["RCV_NO_STATUS"] = "N"; // 收货取消状态  置为 未取消  mfj  20240309
			tmmsm96["RCV_MAT_FLAG"] = "W";
			tmmsm96["EVENT_DESC"] = "自动收货确认等待反馈";

			bcls_rec->Tables["MM0099"].Rows.Add();
			bcls_rec->Tables["MM0099"].Rows[mm0099_count].Merge(tmmsm96);
			mm0099_count++;


			//将数据新增进收货履历表里
			tmmsm33shll.CopyFrom(tmmsm01);
			tmmsm33shll["MAT_WT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm33shll["MAT_ACT_WT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm33shll["QUALIFIED_WT"] = tmmsm01["RECEIVE_WEIGHT"];//合格产量
			tmmsm33shll["MAT_ACT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm33shll["MAT_ACT_LEN"] = tmmsm01["MAT_LEN"];
			tmmsm33shll["MAT_ACT_THICK"] = tmmsm01["MAT_THICK"];
			tmmsm33shll["RCV_MAT_FLAG"] = "W";//N 未收货  W等待(等L4的反馈)  S收货成功


			doFlag = f_mm0011("TMMSM33SHLL_SEQ", 8, v_shll_seq, conn);
			if (doFlag < 0 || v_shll_seq.Trim() == "")
			{
				strcpy(s.msg, "物料材料跟踪号后8位流水号生成错误！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			tmmsm33shll["RESUME_SEQ_NO"] = datetime + v_shll_seq;
			tmmsm33shll.Insert();


			bcls_rec->Tables["MMSMACSH"].Rows.Add();
			bcls_rec->Tables["MMSMACSH"].Rows[mmsmacsh_count].Merge(tmmsm96);
			bcls_rec->Tables["MMSMACSH"].Rows[mmsmacsh_count]["DEAL_FLAG"] = "N";
			mmsmacsh_count++;



			#pragma region 调用仓库接口，进行入库操作   太钢定制
			if (true) //
			{
				bcls_rec->Tables["WM_STOCK"].Rows.Add();				
				bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["MAT_NO"] = tmmsm01["MAT_NO"];
				bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_OPER_ORDER"] = "1B";					//库业务类型
				bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_OPER_ORDER_DIV"] = "1";					//业务类型内区分
				bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_NO"] = "SYA";			//库号
				bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_PLACE_NO"] = "SYA";						//材料库位号
				bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["ROWNO"] = " ";								//行号
				bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["COLUMN_NO"] = " ";							//列号
				bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["LAYERNO"] = 0;								//层号
				bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_PLACE_POSITION"] = "1";				//库位内位置
				wm_stock_count++;
			}
			#pragma endregion	
		}

		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}


		if (bcls_rec->Tables["MMSMACSH"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsmacsh_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		// 调用仓库接口，生成入库队列
		if (bcls_rec->Tables["WM_STOCK"].Rows.get_Count() > 0) {
			Log::Trace("", "", "WM_STOCK", bcls_rec->Tables["WM_STOCK"].Rows.get_Count());
			//doFlag = f_wmsmsm_stock_in(bcls_rec, bcls_ret, conn);  //太钢定制   产出时入库
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}




	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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


	return doFlag;

}
