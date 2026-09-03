/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 吴振楠
日期: 2012-2-17
功能: 按PONO查询板坯信息
修改历史：
 日期:________；修改人：________; 需求提出人________
 变更内容:
**************************************************/  

#include "stdafx.h"
#include "tmmsmis1f.h"  //业务头

using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;


/*<remark>=========================================================
/// <summary>
/// 物料信息查询
/// <para>如果查询在线数据，根据查询条件查询冷轧物料主表TMMCR01</para>
/// <para>如果查询历史数据，根据查询条件查询冷轧物料历史表HMMCR01</para>
/// </summary>
/// <param name="CRRNT_HSTY_FLAG">在线档OR历史档标记</param>
/// <param name="MAT_NO">材料号</param>
/// <param name="FACTORY_DIV">厂别区分</param>
/// <param name="NEXT_UNIT_CODE">下道机组代码</param>
/// <param name="...">其他...</param>
/// <returns>满足查询条件的冷轧物料主表TMMCR01数据</returns>
===========================================================</remark>*/
BM2F_ENTERACE(mmsmis02_inqa)


int f_mmsmis02_inqa(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);//系统日志类定义
	/* 程序内部变量 */
	int doFlag = 0;
	int sql_flag = 0;
	int i = 0;

	CString sqlstr = " ";
	int	blkNum1	= 0;
	int	blkNum	= 0;
	/* 实体类定义 */
	CTMMSMIS1F tmmsmis1f(conn);
	try
	{
		CDbCommand cmdcount1(conn);
		CDbCommand cmdcount2(conn);
		CDbCommand cmdinq(conn);
		CDbCommand cmdinq1(conn);
		CDbCommand cmdinq2(conn);
		CDbCommand cmdinq3(conn);
		CDbCommand cmdinq4(conn);
		CDbCommand comm(conn);
		CDbCommand cmdtime(conn);
		CDbCommand cmdsign(conn);
		CString strSql1 = "";
		CString strSql2 = "";
		CString strSql2p = "";
		CString strSql2t = "";
		CString strSql2h = "";
		CString strSql3 = "";
		CString strSql4 = "";
		CString model_cast_div = "";
		CString o_pono ="";
		CDecimal o_cc_num =0;
		CString o_cast_lot_no ="";
		CString o_heat_no ="";
		CString o_st_no ="";
		CString o_cc_no ="";
		CString o_cast_no ="";
		CString o_cast_div_no ="";
		CString o_cast_time_14 ="";
		CString o_hot_send_div ="";
		CString o_hot_charge_flag ="";
		CString o_odd_decide_result ="";
		CString o_even_decide_result ="";
		CDecimal pono_num =0;
		CDecimal o_raw_slab_wt =0;
		CDecimal o_slab_num =0;

		CDecimal cut_slab_wt =0;
		CDecimal cut_slab_num =0;

		CDecimal cut_slab_wt1 =0;
		CDecimal cut_slab_num1 =0;

		CDecimal hc_slab_wt =0;
		CDecimal hc_slab_num =0;

		CDecimal hc_slab_wt1 =0;
		CDecimal hc_slab_num1 =0;

		CDecimal nom_slab_wt =0;
		CDecimal nom_slab_num =0;

		CDecimal tt_slab_wt =0;
		CDecimal tt_slab_num =0;

		CDecimal ht_slab_wt =0;
		CDecimal ht_slab_num =0;

		CString v_query_flag = bcls_rec->Tables[0].Rows[0]["query_flag"];
		CString v_pono = bcls_rec->Tables[0].Rows[0]["pono"];
		CString v_stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"];
		v_stock_no += "%";
		CString v_mat_status = bcls_rec->Tables[0].Rows[0]["mat_status"];
		CString v_st_no = bcls_rec->Tables[0].Rows[0]["st_no"];
		CString v_direct = bcls_rec->Tables[0].Rows[0]["direct"];
		CString v_ai_time_1_f = bcls_rec->Tables[0].Rows[0]["ai_time_1_f"];
		CString v_ai_time_1_t = bcls_rec->Tables[0].Rows[0]["ai_time_1_t"];
		CString secut_flag = bcls_rec->Tables[0].Rows[0]["secut_flag"];

		int nPageStart = (int)bcls_rec->Tables[1].Rows[0]["PageStart"];
		int nPageSize = (int)bcls_rec->Tables[1].Rows[0]["PageSize"];

		Log::Trace("",__FUNCTION__,"nPageStart = 【{0}】",nPageStart);
		Log::Trace("",__FUNCTION__,"nPageSize = 【{0}】",nPageSize);
		Log::Trace("",__FUNCTION__,"v_query_flag = 【{0}】",(const char*)v_query_flag);
		Log::Trace("",__FUNCTION__,"v_pono = 【{0}】",(const char*)v_pono);
		Log::Trace("",__FUNCTION__,"v_mat_status = 【{0}】",(const char*)v_mat_status);
		Log::Trace("",__FUNCTION__,"v_stock_no = 【{0}】",(const char*)v_stock_no);
		Log::Trace("",__FUNCTION__,"v_st_no = 【{0}】",(const char*)v_st_no);
		Log::Trace("",__FUNCTION__,"v_direct = 【{0}】",(const char*)v_direct);
		Log::Trace("",__FUNCTION__,"v_ai_time_1_f = 【{0}】",(const char*)v_ai_time_1_f);
		Log::Trace("",__FUNCTION__,"v_ai_time_1_t = 【{0}】",(const char*)v_ai_time_1_t);
		Log::Trace("",__FUNCTION__,"secut_flag = 【{0}】",(const char*)secut_flag);
		
		/*查询板坯命令信息 传入块1*/
		/*拼接查询条件*/
		if (v_pono.Trim().GetLength() > 0)
		{
			strSql2p = " AND PONO = '" + v_pono + "'" ;
		}
		if (v_mat_status.Trim().GetLength() > 0)
		{
			strSql2p += " AND MAT_STATUS = '" + v_mat_status + "'" ;
		}
		if (v_stock_no.Trim().GetLength() > 0)
		{
			strSql2p += " AND STOCK_NO like '" + v_stock_no + "'" ;
		}
		if (v_st_no.Trim().GetLength() > 0)
		{
			strSql2p += " AND ST_NO = '" + v_st_no + "'" ;
		}
		if (v_direct.Trim().GetLength() > 0)
		{
			strSql2p += " AND DEST_FIN = '" + v_direct + "'" ;
		}
		if (v_ai_time_1_f.Trim().GetLength() > 0)
		{
			strSql2p += " AND SLAB_CUT_TIME >= '" + v_ai_time_1_f + "'" ;
		}
		if (v_ai_time_1_t.Trim().GetLength() > 0)
		{
			strSql2p += " AND SLAB_CUT_TIME <= '" + v_ai_time_1_t + "'" ;
		}
		if (secut_flag.Trim().Compare("1")==0)
		{
			strSql2p += " AND SECUT_FLAG = ' ' " ;
		}
		else if (secut_flag.Trim().Compare("2")==0)
		{
			strSql2p += " AND SECUT_FLAG BETWEEN '1' AND '2' " ;
		}
		Log::Trace("",__FUNCTION__,"strSql2p = 【{0}】",(const char*)strSql2p);
		/*根据标记查询在线档&历史档  分页*/
		strSql2t = "SELECT COUNT(MAT_NO) FROM TMMSM01 where 1 = 1 ";
		strSql2h = "SELECT COUNT(MAT_NO) FROM HMMSM01 where 1 = 1 ";
		
		strSql2t += strSql2p;
		strSql2h += strSql2p;
		
		Log::Trace("",__FUNCTION__,"strSql2 = 【{0}】",(const char*)strSql2);
		cmdcount1.SetCommandText(strSql2t);
		cmdcount2.SetCommandText(strSql2h);

		/*此方法返回单行单列数据*/
		CDecimal nRecCount = cmdcount1.ExecuteScalar() + cmdcount2.ExecuteScalar() ;
		
		Log::Trace("",__FUNCTION__,"nRecCount = 【{0}】",nRecCount.ToInt32());
		/*返回记录数，分页显示用*/
		bcls_ret->ExtendedProperties.Add("REC_COUNT", nRecCount.ToString());

		/*根据标记查询在线档和历史档*/
		strSql2t = "SELECT ' ' as flag,t.* FROM TMMSM01 t where 1 = 1 ";
		strSql2h = "SELECT '*' as flag,t.* FROM HMMSM01 t where 1 = 1 ";
		
		/*strSql2 += " ORDER BY a.MAT_NO ASC";*/
		strSql2t += strSql2p;
		strSql2h += strSql2p;
		strSql2 = strSql2t + " union all " + strSql2h;
		strSql2 += " ORDER BY MAT_NO  ASC " ;
		Log::Trace("",__FUNCTION__,"strSql2 = 【{0}】",(const char*)strSql2);		
		cmdinq.SetCommandText(strSql2);

		bcls_ret->Tables[0].set_TableName("mmsmis02_inq"); 
		blkNum1=bcls_ret->Tables.IndexOf("mmsmis02_inq"); 
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("mmsmis02_inq");
			blkNum1=bcls_ret->Tables.IndexOf("mmsmis02_inq"); 
		}

		/*此方法是直接压入block数据块，支持分页查询*/
		cmdinq.ExecuteQuery(bcls_ret->Tables["mmsmis02_inq"],nPageStart ,nPageSize);

		bcls_ret->Tables["MMSMIS02_INQ"].Columns.Add(DT_STRING,"OUT_HOT_TIME");
		//20131219新增副钢级1、2、3
		bcls_ret->Tables["MMSMIS02_INQ"].Columns.Add(DT_STRING,"SIGN_CODE_1");
		bcls_ret->Tables["MMSMIS02_INQ"].Columns.Add(DT_STRING,"SIGN_CODE_2");
		bcls_ret->Tables["MMSMIS02_INQ"].Columns.Add(DT_STRING,"SIGN_CODE_3");
		
		CString v_in_stock_hot_time = "";
		CString v_slab_cut_time = "";
		CString v_out_hot_time = "";
		CString sqlstr_outTIME = "";
		//20131219新增
		CString v_sign_code_1 = "";
		CString v_sign_code_2 = "";
		CString v_sign_code_3 = "";
		CString v_order_no = "";
		CString sqlstr_SIGN = "";
		for(int row = 0;row<bcls_ret->Tables["MMSMIS02_INQ"].Rows.get_Count();row++)
		{
			v_in_stock_hot_time = bcls_ret->Tables["MMSMIS02_INQ"].Rows[row]["IN_STOCK_HOT_TIME"];
			v_slab_cut_time = bcls_ret->Tables["MMSMIS02_INQ"].Rows[row]["SLAB_CUT_TIME"];
			v_out_hot_time = "";
			if(v_in_stock_hot_time.Trim().GetLength()==14 && v_slab_cut_time.Trim().GetLength()==14)
			{
				sqlstr_outTIME = "select timestampdiff(8,char(timestamp('"+v_in_stock_hot_time+"','yyyy-MM-DD hh:mm:ss') - timestamp('"+v_slab_cut_time+"','yyyy-MM-DD hh:mm:ss')))  from sysibm.sysdummy1";
				cmdtime.SetCommandText(sqlstr_outTIME);
				cmdtime.ExecuteReader();
				if(cmdtime.Read())
				{
					v_out_hot_time = cmdtime.GetString(1);  //切断时间
					Log::Trace("",__FUNCTION__,"v_out_hot_time b= [{0}]", (const char*)v_out_hot_time);
				}
				cmdtime.Close();
				bcls_ret->Tables["MMSMIS02_INQ"].Rows[row]["OUT_HOT_TIME"] = v_out_hot_time;
			}

			//20131219新增获取合同主表的副钢级代码根据合同号
			v_order_no = bcls_ret->Tables["MMSMIS02_INQ"].Rows[row]["ORDER_NO"];
			v_sign_code_1 = "";
			v_sign_code_2 = "";
			v_sign_code_3 = "";
			if (v_order_no.Trim().GetLength() > 0)
			{
				sqlstr_SIGN = "SELECT  SIGN_CODE_1,SIGN_CODE_2,SIGN_CODE_3  FROM  OMPO.TOM01 WHERE ORDER_NO  = @v_order_no";
				cmdsign.SetCommandText(sqlstr_SIGN);
				cmdsign.Parameters.Set("v_order_no",v_order_no);
				cmdsign.ExecuteReader();
				if(cmdsign.Read())
				{
					v_sign_code_1 = cmdsign.GetString(1);  //副钢级代码1
					v_sign_code_2 = cmdsign.GetString(2);  //副钢级代码2
					v_sign_code_3 = cmdsign.GetString(3);  //副钢级代码3
					Log::Trace("",__FUNCTION__,"v_sign_code_1 = [{0}]", (const char*)v_sign_code_1);
					Log::Trace("",__FUNCTION__,"v_sign_code_3 = [{0}]", (const char*)v_sign_code_3);
				}
				cmdsign.Close();
				bcls_ret->Tables["MMSMIS02_INQ"].Rows[row]["SIGN_CODE_1"] = v_sign_code_1;
				bcls_ret->Tables["MMSMIS02_INQ"].Rows[row]["SIGN_CODE_2"] = v_sign_code_2;
				bcls_ret->Tables["MMSMIS02_INQ"].Rows[row]["SIGN_CODE_3"] = v_sign_code_3;
			}
			
		}

		blkNum=bcls_ret->Tables.IndexOf("TMMSMIS1F"); 
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("TMMSMIS1F");
			blkNum=bcls_ret->Tables.IndexOf("TMMSMIS1F"); 
			bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"PONO");
			bcls_ret->Tables[blkNum].Columns.Add(DT_DECIMAL,"CC_NUM");
			bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"CAST_LOT_NO");
			bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"HEAT_NO");
			bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"ST_NO");
			bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"CC_NO");
			bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"CAST_NO");
			bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"CAST_DIV_NO");
			bcls_ret->Tables[blkNum].Columns.Add(DT_DECIMAL,"CAST_PONO_NUM");
			bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"CAST_TIME_14");
			bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"HOT_SEND_DIV");
			bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"HOT_CHARGE_FLAG");
			bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"ODD_DECIDE_RESULT");
			bcls_ret->Tables[blkNum].Columns.Add(DT_STRING,"EVEN_DECIDE_RESULT");
			bcls_ret->Tables[blkNum].Columns.Add(DT_DECIMAL,"CUT_SLAB_WT");		/*切断量及块数*/
			bcls_ret->Tables[blkNum].Columns.Add(DT_DECIMAL,"CUT_SLAB_NUM");
			bcls_ret->Tables[blkNum].Columns.Add(DT_DECIMAL,"HC_SLAB_WT");		/*热送量及块数*/
			bcls_ret->Tables[blkNum].Columns.Add(DT_DECIMAL,"HC_SLAB_NUM");
			bcls_ret->Tables[blkNum].Columns.Add(DT_DECIMAL,"NOM_SLAB_WT");		/*申请量及块数*/
			bcls_ret->Tables[blkNum].Columns.Add(DT_DECIMAL,"NOM_SLAB_NUM");
			bcls_ret->Tables[blkNum].Columns.Add(DT_DECIMAL,"TT_SLAB_WT");		/*当前档材料总重*/
			bcls_ret->Tables[blkNum].Columns.Add(DT_DECIMAL,"TT_SLAB_NUM");		/*当前档材料总数*/
			bcls_ret->Tables[blkNum].Columns.Add(DT_DECIMAL,"HT_SLAB_WT");		/*历史档材料总重*/
			bcls_ret->Tables[blkNum].Columns.Add(DT_DECIMAL,"HT_SLAB_NUM");		/*历史档材料总数*/
		}

		/*查询制造命令信息 传入块2*/
		if (v_pono.Trim().GetLength() <= 0)
		{
			Log::Trace("",__FUNCTION__,"PONO 【{0}】为空",(const char*)v_pono);
			bcls_ret->Tables[blkNum].Rows.Add();
			bcls_ret->Tables[blkNum].Rows[0]["PONO"] = " ";
			bcls_ret->Tables[blkNum].Rows[0]["CC_NUM"] = "0";
			bcls_ret->Tables[blkNum].Rows[0]["CAST_LOT_NO"] = " ";
			bcls_ret->Tables[blkNum].Rows[0]["HEAT_NO"] = " ";
			bcls_ret->Tables[blkNum].Rows[0]["ST_NO"] = " ";
			bcls_ret->Tables[blkNum].Rows[0]["CC_NO"] = " ";
			bcls_ret->Tables[blkNum].Rows[0]["CAST_NO"] = " ";
			bcls_ret->Tables[blkNum].Rows[0]["CAST_DIV_NO"] = " ";
			bcls_ret->Tables[blkNum].Rows[0]["CAST_PONO_NUM"] = 0;
			bcls_ret->Tables[blkNum].Rows[0]["CAST_TIME_14"] = " ";
			bcls_ret->Tables[blkNum].Rows[0]["HOT_SEND_DIV"] = " ";
			bcls_ret->Tables[blkNum].Rows[0]["HOT_CHARGE_FLAG"] = " ";
			bcls_ret->Tables[blkNum].Rows[0]["ODD_DECIDE_RESULT"] = " ";
			bcls_ret->Tables[blkNum].Rows[0]["EVEN_DECIDE_RESULT"] = " ";
			bcls_ret->Tables[blkNum].Rows[0]["CUT_SLAB_WT"] = "0";
			bcls_ret->Tables[blkNum].Rows[0]["CUT_SLAB_NUM"] = "0";
			bcls_ret->Tables[blkNum].Rows[0]["HC_SLAB_WT"] = "0";
			bcls_ret->Tables[blkNum].Rows[0]["HC_SLAB_NUM"] = "0";
			bcls_ret->Tables[blkNum].Rows[0]["NOM_SLAB_WT"] = "0";
			bcls_ret->Tables[blkNum].Rows[0]["NOM_SLAB_NUM"] = "0";
		}
		else
		{
		    sqlstr =  "SELECT PONO,CC_NUM,CAST_LOT_NO,HEAT_NO,ST_NO,CC_NO,CAST_NO,CAST_DIV_NO,CAST_TIME_14,HOT_SEND_DIV,HOT_CHARGE_FLAG,ODD_DECIDE_RESULT,EVEN_DECIDE_RESULT"
							  ",RAW_SLAB_WT,SLAB_NUM"
							  " FROM TMMSMIS1F  where 1 = 1 "
							  " AND PONO = '" + v_pono + "'" ;

			comm.SetCommandText(sqlstr);
			comm.ExecuteReader();
			while (comm.Read())			/*cmdinq1*/
			{
				o_pono = comm.GetString(1);
				o_cc_num = comm.GetDecimal(2);
				o_cast_lot_no = comm.GetString(3);
				o_heat_no = comm.GetString(4);
				o_st_no = comm.GetString(5);
				o_cc_no = comm.GetString(6);
				o_cast_no = comm.GetString(7) ;
				o_cast_div_no = comm.GetString(8);
				o_cast_time_14 = comm.GetString(9);
				o_hot_send_div = comm.GetString(10);
				o_hot_charge_flag = comm.GetString(11);
				o_odd_decide_result = comm.GetString(12);
				o_even_decide_result = comm.GetString(13);
				cut_slab_wt = comm.GetDecimal(14);
				cut_slab_num = comm.GetDecimal(15);
				Log::Trace("",__FUNCTION__,"o_pono = 【{0}】",(const char*)o_pono);
				Log::Trace("",__FUNCTION__,"o_cc_num = 【{0}】",o_cc_num);											
				Log::Trace("",__FUNCTION__,"o_cast_lot_no = 【{0}】",o_cast_lot_no);								
				Log::Trace("",__FUNCTION__,"o_heat_no = 【{0}】",(const char*)o_heat_no);							/*出钢钢号*/
				Log::Trace("",__FUNCTION__,"o_st_no = 【{0}】",(const char*)o_st_no);								/*出钢记号*/
				Log::Trace("",__FUNCTION__,"o_cc_no = 【{0}】",(const char*)o_cc_no);								/*连连铸实绩（预定）*/
				Log::Trace("",__FUNCTION__,"o_cast_no = 【{0}】",(const char*)o_cast_no);							
				Log::Trace("",__FUNCTION__,"o_cast_div_no = 【{0}】",(const char*)o_cast_div_no);					/*CAST分割NO*/
				Log::Trace("",__FUNCTION__,"o_cast_time_14 = 【{0}】",(const char*)o_cast_time_14);
				Log::Trace("",__FUNCTION__,"o_hot_send_div = 【{0}】",(const char*)o_hot_send_div);					/*热送区分*/
				Log::Trace("",__FUNCTION__,"o_hot_charge_flag = 【{0}】",(const char*)o_hot_charge_flag);			/*热送标记*/
				Log::Trace("",__FUNCTION__,"o_odd_decide_result = 【{0}】",(const char*)o_odd_decide_result);		/*奇流ISE判定结果*/
				Log::Trace("",__FUNCTION__,"o_even_decide_result = 【{0}】",(const char*)o_even_decide_result);
				Log::Trace("",__FUNCTION__,"cut_slab_wt = 【{0}】",cut_slab_wt);								/*粗坯量*/
				Log::Trace("",__FUNCTION__,"cut_slab_num = 【{0}】",cut_slab_num);										/*板坯数量*/
				strSql4 = "SELECT COUNT (PONO) FROM TMMSMIS1F WHERE CAST_NO = @cast_no ";
				cmdinq1.SetCommandText(strSql4);
				cmdinq1.Parameters.Set("cast_no",o_cast_no);
				cmdinq1.ExecuteReader();
				if (cmdinq1.Read())			
				{
					pono_num = cmdinq1.GetDecimal(1);
				}
				cmdinq1.Close();
			}
			comm.Close();
			
			bcls_ret->Tables[blkNum].Rows.Add();
			bcls_ret->Tables[blkNum].Rows[0]["CUT_SLAB_WT"] = "0";
			bcls_ret->Tables[blkNum].Rows[0]["CUT_SLAB_NUM"] = "0";
			bcls_ret->Tables[blkNum].Rows[0]["HC_SLAB_WT"] = "0";
			bcls_ret->Tables[blkNum].Rows[0]["HC_SLAB_NUM"] = "0";
			bcls_ret->Tables[blkNum].Rows[0]["NOM_SLAB_WT"] = "0";
			bcls_ret->Tables[blkNum].Rows[0]["NOM_SLAB_NUM"] = "0";
			bcls_ret->Tables[blkNum].Rows[0]["PONO"] = o_pono;
			bcls_ret->Tables[blkNum].Rows[0]["CC_NUM"] = o_cc_num;
			bcls_ret->Tables[blkNum].Rows[0]["CAST_LOT_NO"] = o_cast_lot_no;
			bcls_ret->Tables[blkNum].Rows[0]["CAST_PONO_NUM"] = pono_num;
			bcls_ret->Tables[blkNum].Rows[0]["HEAT_NO"] = o_heat_no;
			bcls_ret->Tables[blkNum].Rows[0]["ST_NO"] = o_st_no;
			bcls_ret->Tables[blkNum].Rows[0]["CC_NO"] = o_cc_no;
			bcls_ret->Tables[blkNum].Rows[0]["CAST_NO"] = o_cast_no;
			bcls_ret->Tables[blkNum].Rows[0]["CAST_DIV_NO"] = o_cast_div_no;
			bcls_ret->Tables[blkNum].Rows[0]["CAST_TIME_14"] = o_cast_time_14;
			bcls_ret->Tables[blkNum].Rows[0]["HOT_SEND_DIV"] = o_hot_send_div;
			bcls_ret->Tables[blkNum].Rows[0]["HOT_CHARGE_FLAG"] = o_hot_charge_flag;
			bcls_ret->Tables[blkNum].Rows[0]["ODD_DECIDE_RESULT"] = o_odd_decide_result;
			bcls_ret->Tables[blkNum].Rows[0]["EVEN_DECIDE_RESULT"] = o_even_decide_result;
			bcls_ret->Tables[blkNum].Rows[0]["CUT_SLAB_WT"] = cut_slab_wt;
			bcls_ret->Tables[blkNum].Rows[0]["CUT_SLAB_NUM"] = cut_slab_num;

			tt_slab_wt = 0;
			tt_slab_num = 0;
			ht_slab_wt = 0;
			ht_slab_num = 0;
			//计算切断量
			strSql1 = CString("SELECT SUM(MAT_ACT_WT),COUNT(MAT_NO) " 
				" FROM TMMSM01 a  where 1 = 1 "
				" AND PONO = @v_pono"
				//" AND ST_NO LIKE @v_st_no"
				);
			cmdinq2.SetCommandText(strSql1);
			cmdinq2.Parameters.Set("v_pono",v_pono);
			//cmdinq2.Parameters.Set("v_st_no",v_st_no);
			cmdinq2.ExecuteReader();
			while (cmdinq2.Read())			/*cmdinq1*/
			{
				tt_slab_wt = cmdinq2.GetDecimal(1);
				tt_slab_num = cmdinq2.GetDecimal(2);
			}
			cmdinq2.Close();
			Log::Trace("",__FUNCTION__,"计算历史切断量");		
			//计算历史切断量
			strSql1 = CString("SELECT SUM(MAT_ACT_WT),COUNT(MAT_NO) " 
				" FROM HMMSM01 a  where 1 = 1 "
				" AND PONO = @v_pono"
				//" AND ST_NO LIKE @v_st_no"
				);
			cmdinq2.SetCommandText(strSql1);
			cmdinq2.Parameters.Set("v_pono",v_pono);
			//cmdinq2.Parameters.Set("v_st_no",v_st_no);
			cmdinq2.ExecuteReader();
			while (cmdinq2.Read())			/*cmdinq1*/
			{
				ht_slab_wt = cmdinq2.GetDecimal(1);
				ht_slab_num = cmdinq2.GetDecimal(2);
			}
			cmdinq2.Close();
			bcls_ret->Tables[blkNum].Rows[0]["TT_SLAB_WT"] = tt_slab_wt;
			bcls_ret->Tables[blkNum].Rows[0]["TT_SLAB_NUM"] = tt_slab_num;
			bcls_ret->Tables[blkNum].Rows[0]["HT_SLAB_WT"] = ht_slab_wt;
			bcls_ret->Tables[blkNum].Rows[0]["HT_SLAB_NUM"] = ht_slab_num;
			//计算热送量
			strSql1 = CString("SELECT SUM(MAT_ACT_WT),COUNT(MAT_NO) " 
							  " FROM TMMSM01  where 1 = 1 "
							  " AND PONO = '" + v_pono + "'"
							  " AND HOT_SEND_FLAG_ACT = '1'"
							  " AND ST_NO LIKE '" + v_st_no + "%'"
							 );
			cmdinq3.SetCommandText(strSql1);
			Log::Trace("",__FUNCTION__,"计算热送量 strSql1 = 【{0}】",(const char*)strSql1);
			cmdinq3.ExecuteReader();
			
			if (cmdinq3.Read())			/*cmdinq1*/
			{
				
				hc_slab_wt1 = cmdinq3.GetDecimal(1);
				hc_slab_num1 = cmdinq3.GetDecimal(2);	

				Log::Trace("",__FUNCTION__,"热送量当前 hc_slab_wt1 = 【{0}】",hc_slab_wt1.ToInt32());
				Log::Trace("",__FUNCTION__,"热送量当前 hc_slab_num1 = 【{0}】",hc_slab_num1.ToInt32());
				
				//计算历史热送量
				strSql1 = CString("SELECT SUM(MAT_ACT_WT),COUNT(MAT_NO) " 
								  " FROM HMMSM01  where 1 = 1 "
								  " AND PONO = '" + v_pono + "'"
								  " AND HOT_SEND_FLAG_ACT = '1'"
								  " AND ST_NO LIKE '" + v_st_no + "%'"
								 );
				cmdinq4.SetCommandText(strSql1);
				Log::Trace("",__FUNCTION__,"计算历史热送量 strSql1 = 【{0}】",(const char*)strSql1);

				cmdinq4.ExecuteReader();
				
				if (cmdinq4.Read())			/*cmdinq1*/
				{
					Log::Trace("",__FUNCTION__,"历史热送量 hc_slab_wt = 【{0}】",hc_slab_wt.ToInt32());
					Log::Trace("",__FUNCTION__,"历史热送量 hc_slab_num = 【{0}】",hc_slab_num.ToInt32());
					hc_slab_wt = cmdinq3.GetDecimal(1);
					hc_slab_num = cmdinq3.GetDecimal(2);	
				}
				cmdinq4.Close();
				hc_slab_wt = hc_slab_wt + hc_slab_wt1;
				hc_slab_num = hc_slab_num + hc_slab_num1;
				Log::Trace("",__FUNCTION__,"计算后热送量 hc_slab_wt = 【{0}】",hc_slab_wt.ToInt32());
				Log::Trace("",__FUNCTION__,"计算后热送量 hc_slab_num = 【{0}】",hc_slab_num.ToInt32());

				bcls_ret->Tables[blkNum].Rows[0]["HC_SLAB_WT"] = hc_slab_wt;
				bcls_ret->Tables[blkNum].Rows[0]["HC_SLAB_NUM"] = hc_slab_num;
			}

			 cmdinq3.Close();

			//计算申请量
			int query_r= 0;
			if(v_pono.Trim().Compare("400001")>=0 && v_pono.Trim().Compare("449999")<=0 )
			{
				strSql1 = CString(" SELECT PLAN_TAP_WT,(FLOWN_SLAB_NUM_1 + FLOWN_SLAB_NUM_2) " 
							  " FROM TPMOUHP38  where 1 = 1 "
							   " AND PONO = @v_pono"
							/*  " AND ST_NO LIKE @v_st_no"*/
							 );
				query_r = 0;
			}
			else
			{	
				strSql1 = CString("SELECT NOM_SLAB_WT,INGOT_UNIT_WT,MAT_NO,MODEL_CAST_DIV " 
							  " FROM TPMOM00   where 1 = 1 "
							    " AND PONO = @v_pono"
							  //" AND ST_NO LIKE @v_st_no"
							 );
				query_r = 1;
			}
			cmdinq4.SetCommandText(strSql1);
			Log::Trace("",__FUNCTION__,"计算申请量 strSql1 = 【{0}】",strSql1);
			
			cmdinq4.Parameters.Set("v_pono",v_pono);
			//cmdinq4.Parameters.Set("v_st_no",v_st_no);
			cmdinq4.ExecuteReader();
			while (cmdinq4.Read())			/*cmdinq1*/
			{
				if(query_r == 1)
				{
					if(model_cast_div.Trim()=="1")
					{
						model_cast_div = "";
						nom_slab_wt = nom_slab_wt + cmdinq4.GetDecimal(1);
						nom_slab_num = nom_slab_num+1;	
					}
					else
					{
						nom_slab_wt = nom_slab_wt + cmdinq4.GetDecimal(2);
						nom_slab_num = nom_slab_num+1;
					}
				}
				else
				{
					nom_slab_wt = cmdinq4.GetDecimal(1);
					nom_slab_num = cmdinq4.GetDecimal(2);	
				}
				model_cast_div = "";
			}
			Log::Trace("",__FUNCTION__,"计算申请量 nom_slab_wt = 【{0}】",nom_slab_wt.ToInt32());
			Log::Trace("",__FUNCTION__,"计算申请量 nom_slab_num = 【{0}】",nom_slab_num.ToInt32());
			
			bcls_ret->Tables[blkNum].Rows[0]["NOM_SLAB_WT"] = nom_slab_wt;
			bcls_ret->Tables[blkNum].Rows[0]["NOM_SLAB_NUM"] = nom_slab_num;


		}
		
		
		

		

		
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return(doFlag);
}

