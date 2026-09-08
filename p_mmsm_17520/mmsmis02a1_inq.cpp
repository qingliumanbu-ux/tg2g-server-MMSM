/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 吴振楠
日期: 2012-2-17
功能: 按PONO查询板坯信息
修改历史：
 日期:________；修改人：________; 需求提出人DEST
 变更内容:
**************************************************/  
#include "stdafx.h"
#include "tmmsmis1f.h"  //业务头
#include "mmhrzg_common.h"

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
BM2F_ENTERACE(mmsmis02a1_inq)

int f_mmsmis02a1_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	
	CTracer log(__FUNCTION__);//系统日志类定义
	/* 程序内部变量 */
	int doFlag = 0;
	int sql_flag = 0;
	int i = 0;
	/*CString __FUNCTION__ ="mmsmis02a1_inq";*/
	CString sqlstr = " ";
	int	blkNum1	= 0;
	int	blkNum	= 0;
	int l_max = 0;
	/* 实体类定义 */
	CTMMSMIS1F tmmsmis1f(conn);
	try
	{
		CDbCommand cmdcount(conn);
		CDbCommand cmdinq(conn);
		CDbCommand cmdinq1(conn);
		CDbCommand cmdinq2(conn);
		CDbCommand cmdinq3(conn);
		CDbCommand cmdinq4(conn);
		CDbCommand comma(conn);
		CDbCommand com(conn);
		CString strSql = "";		
		CString strSql1 = "";
		CString strSql2 = "";
		CString strSql2p = "";
		CString strSql2t = "";
		CString strSql2h = "";
		CString strSql3 = "";
		CString strSql4 = "";
		CString sqlstrt1 = "";
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
		CDecimal o_raw_slab_wt =0;
		CDecimal o_slab_num =0;
        CDecimal L_N = 0;
		CDecimal cut_slab_wt =0;
		CDecimal cut_slab_num =0;
		CDecimal hc_slab_wt =0;
		CDecimal hc_slab_num =0;
		CDecimal nom_slab_wt =0;
		CDecimal nom_slab_num =0;
		CDecimal tt_slab_wt =0;
		CDecimal tt_slab_num =0;
		CDecimal ht_slab_wt =0;
		CDecimal ht_slab_num =0;
		CDecimal in_stock_time_dura =0;
	    CString hot_charge_method_temp = "";
		CString v_mat_no = "";
	    CString v_dest_fin = "";
		CString b_band_ord_sort = "";
		CString v_st_no_temp = "";
		CString v_in_stock_hot_time = "";
		CString v_slab_cut_time = "";
	    CString v_order_no = "";
		CString sqlstr_COLDTIME = "";
		CString v_query_flag = bcls_rec->Tables[0].Rows[0]["query_flag"];
		//20130828新增是否查询炉次主档条件
		CString v_query_is1f = bcls_rec->Tables[0].Rows[0]["query_is1f"];

		//20140912新增是否清理标志 
		CString v_clean_if_flag = bcls_rec->Tables[0].Rows[0]["clean_if_flag"];

		//20150821新增IBB标志 
		CString ibb_flag = bcls_rec->Tables[0].Rows[0]["ibb_flag"];
		//20160129新增必热送标志 
		CString hot_flag = bcls_rec->Tables[0].Rows[0]["hot_flag"];


		CString backlog_tmp = "";
		//20140703新增行数判断和制造命令号
		int  row_count = bcls_rec->Tables[0].Rows.get_Count();
		CString pono = bcls_rec->Tables[0].Rows[0]["pono"].ToString().Trim();
		CString v_pono = "";

		/*CString v_pono = bcls_rec->Tables[0].Rows[0]["pono"];*/
		CString v_stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"];
		if(v_stock_no.Trim().GetLength()<3 && v_stock_no.Trim().GetLength()>0)
		{
		    v_stock_no = v_stock_no+"%";
		}
		CString v_backlog = "";
		CString i_stock_no = "";
		CString i_dest_fin = "";
		//CString v_mat_status = bcls_rec->Tables[0].Rows[0]["mat_status"];
		CString v_st_no = bcls_rec->Tables[0].Rows[0]["st_no"];
		/*v_st_no += "%";*/
		CString v_direct = bcls_rec->Tables[0].Rows[0]["direct"];

		/*CString v_mat_status = bcls_rec->Tables[0].Rows[0]["mat_status"];*/
		CString v_store_area = bcls_rec->Tables[0].Rows[0]["store_area"];

		CString v_ai_time_1_f = bcls_rec->Tables[0].Rows[0]["ai_time_1_f"];
		CString v_ai_time_1_t = bcls_rec->Tables[0].Rows[0]["ai_time_1_t"];
		CString secut_flag = bcls_rec->Tables[0].Rows[0]["secut_flag"];
		CString v_backlog56 = bcls_rec->Tables[0].Rows[0]["backlog56"];
		CString v_backlog78 = bcls_rec->Tables[0].Rows[0]["backlog78"];
		CString v_backlog9a = bcls_rec->Tables[0].Rows[0]["backlog9a"];
		CString v_backlogdf = bcls_rec->Tables[0].Rows[0]["backlogdf"];
		CString whole_backlog_code1 = bcls_rec->Tables[0].Rows[0]["whole_backlog_code1"];
		CString whole_backlog_code2 = bcls_rec->Tables[0].Rows[0]["whole_backlog_code2"];
		CString whole_backlog_code3 = bcls_rec->Tables[0].Rows[0]["whole_backlog_code3"];
		CString product_flag = bcls_rec->Tables[0].Rows[0]["PRODUCT_FLAG"];
		CString hot_test_code = bcls_rec->Tables[0].Rows[0]["hot_test_code"];
		CString orderNo = bcls_rec->Tables[0].Rows[0]["order_no"];
		CString orderby = bcls_rec->Tables[0].Rows[0]["orderby"];
		CString mat_group = bcls_rec->Tables[0].Rows[0]["mat_group"];
		CString quality_grade = bcls_rec->Tables[0].Rows[0]["quality_grade"];
		CString orderby_sql = "";
		if(orderby.Trim()=="1")
		{
			orderby_sql = " ORDER BY ST_NO ASC,MAT_ACT_WIDTH ASC ,MAT_NO ASC ";
		}
		else if(orderby.Trim()=="2")
		{
			orderby_sql = " ORDER BY STOCK_NO ASC ,MAT_NO ASC ";
		}
		else if(orderby.Trim()=="3")
		{
			orderby_sql = " ORDER BY MAT_NO ASC ";
		}
		CString st_no_flag = bcls_rec->Tables[0].Rows[0]["st_no_flag"];
        CString in_stock_no = "";
		//20160218 增加热装方式
		CString hot_charge_method = "";

		//20130507新增3个中间量
		CString v_whole_backlog_code1 = "%" + whole_backlog_code1 + "%" ;
		CString v_whole_backlog_code2 = "%" + whole_backlog_code2 + "%" ;
		CString v_whole_backlog_code3 = "%" + whole_backlog_code3 + "%" ;

		//20170302新增预匹配标记
		CString form_plate_flag = bcls_rec->Tables[0].Rows[0]["form_plate_flag"];

		//20170322新增二切计划标记
		CString secut_plan_no = bcls_rec->Tables[0].Rows[0]["secut_plan_no"];

		//20170421按周交货标记和催货日期 
		CString  delivy_week_flag = "";
		CString  confm_prg_plan_date = "";

		int nPageStart = (int)bcls_rec->Tables[1].Rows[0]["PageStart"];
		int nPageSize = (int)bcls_rec->Tables[1].Rows[0]["PageSize"];
		Log::Trace("",__FUNCTION__,"nPageStart = 【{0}】",nPageStart);
		Log::Trace("",__FUNCTION__,"nPageSize = 【{0}】",nPageSize);
		Log::Trace("",__FUNCTION__,"v_query_flag = 【{0}】",(const char*)v_query_flag);
		Log::Trace("",__FUNCTION__,"v_pono = 【{0}】",(const char*)v_pono);
		/*Log::Trace("",__FUNCTION__,"v_mat_status = 【{0}】",(const char*)v_mat_status);*/
		Log::Trace("",__FUNCTION__,"v_stock_no = 【{0}】",(const char*)v_stock_no);
		Log::Trace("",__FUNCTION__,"v_st_no = 【{0}】",(const char*)v_st_no);
		Log::Trace("",__FUNCTION__,"v_direct = 【{0}】",(const char*)v_direct);
		Log::Trace("",__FUNCTION__,"v_ai_time_1_f = 【{0}】",(const char*)v_ai_time_1_f);
		Log::Trace("",__FUNCTION__,"v_ai_time_1_t = 【{0}】",(const char*)v_ai_time_1_t);
		Log::Trace("",__FUNCTION__,"secut_flag = 【{0}】",(const char*)secut_flag);
		Log::Trace("",__FUNCTION__,"v_backlog56 = 【{0}】",(const char*)v_backlog56);
		Log::Trace("",__FUNCTION__,"v_backlog78 = 【{0}】",(const char*)v_backlog78);
		Log::Trace("",__FUNCTION__,"v_backlog9a = 【{0}】",(const char*)v_backlog9a);
		Log::Trace("",__FUNCTION__,"v_backlogdf = 【{0}】",(const char*)v_backlogdf);
		Log::Trace("",__FUNCTION__,"orderby = 【{0}】",(const char*)orderby);
		Log::Trace("",__FUNCTION__,"st_no_flag = 【{0}】",(const char*)st_no_flag);
		Log::Trace("",__FUNCTION__,"v_query_is1f = 【{0}】",(const char*)v_query_is1f);
		Log::Trace("",__FUNCTION__,"v_clean_if_flag = 【{0}】",(const char*)v_clean_if_flag);
		Log::Trace("", __FUNCTION__, "v_store_area = 【{0}】", (const char*)v_store_area);

        bcls_ret->Tables[0].set_TableName("MMSMIS02A1_INQ"); 
       
		CString v_order_status = "";
		if(bcls_rec->Tables[0].Columns.Contains("order_status"))
		{
			v_order_status =  bcls_rec->Tables[0].Rows[0]["order_status"];
			Log::Trace("",__FUNCTION__,"传入参数v_order_status = 【{0}】",(const char*)v_order_status);
		}
		CString v_mat_status = "";
		if(bcls_rec->Tables[0].Columns.Contains("mat_status"))
		{
			v_mat_status =  bcls_rec->Tables[0].Rows[0]["mat_status"];
			Log::Trace("",__FUNCTION__,"传入参数v_mat_status = 【{0}】",(const char*)v_mat_status);
		}
		CString v_band_ord_sort = "";
		if(bcls_rec->Tables[0].Columns.Contains("band_ord_sort"))
		{
			v_band_ord_sort =  bcls_rec->Tables[0].Rows[0]["band_ord_sort"];
			Log::Trace("",__FUNCTION__,"传入参数v_band_ord_sort = 【{0}】",(const char*)v_band_ord_sort);
		}
		/*blkNum=bcls_ret->Tables.IndexOf("MMSMIS02_INQ"); 
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("MMSMIS02_INQ");
			blkNum1=bcls_ret->Tables.IndexOf("MMSMIS02_INQ"); 
		}	*/
		
		/*查询板坯命令信息 传入块1*/
		if (!bcls_rec->Tables.Contains("STOCK_NO_IN"))
	    {
		    bcls_rec->Tables.Add("STOCK_NO_IN");
		    bcls_rec->Tables["STOCK_NO_IN"].Columns.Add(DT_STRING,"STOCK_NO");
	    }
	    if (!bcls_rec->Tables.Contains("DEST"))
	    {
		    bcls_rec->Tables.Add("DEST");
		    bcls_rec->Tables["DEST"].Columns.Add(DT_STRING,"DEST_FIN");
	    }
		
		/*拼接查询条件*/
		/*if (v_pono.Trim().GetLength() > 0)
		{
			strSql2p += " AND a.PONO like @pono " ;
		}*/
		/*if (v_mat_status.Trim().GetLength() > 0 && v_mat_status.Trim().GetLength() <= 2)
		{
			strSql2p += " AND a.MAT_STATUS like @mat_status" ;
		}
		else if(v_mat_status.Trim().GetLength() > 0)
		{
			strSql2p += " AND (a.MAT_STATUS like @mat_status1 OR a.MAT_STATUS like @mat_status2) " ;
		}*/

		Log::Trace("", __FUNCTION__, "form_plate_flag = 【{0}】", (const char*)form_plate_flag);

		if (v_st_no.Trim().GetLength() > 0)
		{
			strSql2p += " AND a.ST_NO LIKE @v_st_no||'%' " ;
		}

		if (v_store_area.Trim().GetLength() > 0)
		{
			strSql2p += " AND a.STORE_AREA LIKE @v_store_area||'%' ";
		}

		if (v_ai_time_1_f.Trim().GetLength() > 0)
		{
			strSql2p += " AND a.SLAB_CUT_TIME >= @ai_time_1_fr" ;
		}
		if (v_ai_time_1_t.Trim().GetLength() > 0)
		{
			strSql2p += " AND a.SLAB_CUT_TIME <= @ai_time_1_to" ;
		}
		/*if (v_pono.Trim().GetLength() > 0)
		{
			strSql2p += " AND a.PONO like @pono " ;
		}*/
		if(orderNo.Trim().GetLength() > 0 && orderNo.Trim() == "Y" )
		{
			strSql2p += " AND trim(a.ORDER_NO) > ' ' " ;
		}
		if(orderNo.Trim().GetLength() > 0 && orderNo.Trim() == "N" )
		{
			strSql2p += " AND trim(a.ORDER_NO) <= ' ' " ;
		}
		if(orderNo.Trim().GetLength() > 0 && orderNo.Trim() != "N" && orderNo.Trim() != "Y" )
		{
			strSql2p += " AND a.ORDER_NO = @order_no " ;
		}

		if(whole_backlog_code1.Trim().GetLength() > 0 )
		{
			Log::Trace("",__FUNCTION__,"传入参数whole_backlog_code1 = 【{0}】",(const char*)whole_backlog_code1);
			Log::Trace("",__FUNCTION__,"传入参数v_whole_backlog_code1 = 【{0}】",(const char*)v_whole_backlog_code1);
// DM8 适配 CHANGE-365:查询。见改写原因。
// 改写原因：DB2 POSSTR(源串,子串) 改为 DM 的 INSTR(源串,子串)(参数顺序一致,未找到时同为 0;POSSTR 不在 DM 函数手册,INSTR 为官方字符串函数)；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
			// strSql2p += " AND ((a.WHOLE_BACKLOG like @v_whole_backlog_code1 " 
			            // " and (POSSTR(a.WHOLE_BACKLOG,@whole_backlog_code1) - POSSTR(a.WHOLE_BACKLOG,@whole_backlog_code1)/2 *2) > 0 ) ";
// DM8 SQL：
			strSql2p += " AND ((a.WHOLE_BACKLOG like @v_whole_backlog_code1 " 
			            " and (INSTR(a.WHOLE_BACKLOG, @whole_backlog_code1) - INSTR(a.WHOLE_BACKLOG, @whole_backlog_code1)/2 *2) > 0 ) ";
		}

		if(whole_backlog_code2.Trim().GetLength() > 0 )
		{
			Log::Trace("",__FUNCTION__,"传入参数whole_backlog_code2 = 【{0}】",(const char*)whole_backlog_code2);
			Log::Trace("",__FUNCTION__,"传入参数v_whole_backlog_code2 = 【{0}】",(const char*)v_whole_backlog_code2);
// DM8 适配 CHANGE-366:查询。见改写原因。
// 改写原因：DB2 POSSTR(源串,子串) 改为 DM 的 INSTR(源串,子串)(参数顺序一致,未找到时同为 0;POSSTR 不在 DM 函数手册,INSTR 为官方字符串函数)；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
			// strSql2p += " OR (a.WHOLE_BACKLOG like @v_whole_backlog_code2 " 
			            // " and (POSSTR(a.WHOLE_BACKLOG,@whole_backlog_code2) - POSSTR(a.WHOLE_BACKLOG,@whole_backlog_code2)/2 *2) > 0 ) ";
// DM8 SQL：
			strSql2p += " OR (a.WHOLE_BACKLOG like @v_whole_backlog_code2 " 
			            " and (INSTR(a.WHOLE_BACKLOG, @whole_backlog_code2) - INSTR(a.WHOLE_BACKLOG, @whole_backlog_code2)/2 *2) > 0 ) ";
		}

		if(whole_backlog_code3.Trim().GetLength() > 0 )
		{
			Log::Trace("",__FUNCTION__,"传入参数whole_backlog_code3 = 【{0}】",(const char*)whole_backlog_code3);
			Log::Trace("",__FUNCTION__,"传入参数v_whole_backlog_code3 = 【{0}】",(const char*)v_whole_backlog_code3);
// DM8 适配 CHANGE-367:查询。见改写原因。
// 改写原因：DB2 POSSTR(源串,子串) 改为 DM 的 INSTR(源串,子串)(参数顺序一致,未找到时同为 0;POSSTR 不在 DM 函数手册,INSTR 为官方字符串函数)；依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
			// strSql2p += " OR (a.WHOLE_BACKLOG like @v_whole_backlog_code3 " 
			        	// " and (POSSTR(a.WHOLE_BACKLOG,@whole_backlog_code3) - POSSTR(a.WHOLE_BACKLOG,@whole_backlog_code3)/2 *2) > 0 ) ";
// DM8 SQL：
			strSql2p += " OR (a.WHOLE_BACKLOG like @v_whole_backlog_code3 " 
			        	" and (INSTR(a.WHOLE_BACKLOG, @whole_backlog_code3) - INSTR(a.WHOLE_BACKLOG, @whole_backlog_code3)/2 *2) > 0 ) ";
		}

		if(whole_backlog_code1.Trim().GetLength() > 0 || whole_backlog_code2.Trim().GetLength() > 0 || whole_backlog_code3.Trim().GetLength() > 0)
		{
			strSql2p += ")";
		}
		if (v_stock_no.Trim().GetLength() > 0)
		{
		    if(v_stock_no.Trim().GetLength() < 5)
		    {
		        strSql2p = strSql2p + " AND a.STOCK_NO LIKE @stock_no ";
		    }
		    else
		    {
		        l_max = (v_stock_no.Trim().GetLength()+2)/5;
			    for(int l = 0;l<l_max;l++)
			    {
				    bcls_rec->Tables["STOCK_NO_IN"].Rows.Add();	
				    bcls_rec->Tables["STOCK_NO_IN"].Rows[l]["STOCK_NO"] = v_stock_no.Trim().Substring(l*5,3);
				    i_stock_no = "stock_no" + L_N.ToString();
				    if(l==0)
				    {
					    strSql2p = strSql2p + " AND (a.STOCK_NO LIKE @";
					    strSql2p = strSql2p	+  i_stock_no ;
				    }
				    else
				    {
					    strSql2p = strSql2p + " OR a.STOCK_NO LIKE @" ;
					    strSql2p = strSql2p	+  i_stock_no ;
				    }
				    L_N = L_N + 1;
		        }
		        strSql2p = strSql2p + ") ";
		    }
		 }
		if (v_direct.Trim().GetLength() > 0)
		{
		    L_N = 0;
            l_max = (v_direct.Trim().GetLength()+2)/3;
            for(int l = 0;l<l_max;l++)
            {
                bcls_rec->Tables["DEST"].Rows.Add();
                bcls_rec->Tables["DEST"].Rows[l]["DEST_FIN"] =  v_direct.Trim().Substring(l*3,2);               
                i_dest_fin = "dest_fin" + L_N.ToString();
                if(l==0)
                {
                    strSql2p = strSql2p + " AND (a.DEST_FIN LIKE @";
                    strSql2p = strSql2p	+  i_dest_fin ;
                }
                else
                {
                    strSql2p = strSql2p + " OR a.DEST_FIN LIKE @" ;
                    strSql2p = strSql2p	+  i_dest_fin ;
                }
            L_N = L_N + 1;
            }
            strSql2p = strSql2p + ") ";
			/*strSql2p += " AND DEST_FIN = @dest_fin" ;*/
		}
		if (v_order_status.Trim().GetLength() > 0)
		{
            l_max = (v_order_status.Trim().GetLength()+2)/4;
			Log::Trace("",__FUNCTION__,"v_order_status.l_max = 【{0}】len[{1}]",l_max,v_order_status.Trim().GetLength());
            for(int l = 0;l<l_max;l++)
            {
				if(l == 0)
				{
					strSql2p = strSql2p + " AND b.ORDER_STATUS in ('" + v_order_status.Trim().Substring(l*4,2) + "'";
				}
				else
				{
					strSql2p += ",'" + v_order_status.Trim().Substring(l*4,2) + "'";
				}
            }

            strSql2p = strSql2p + ") ";
			//Log::Trace("",__FUNCTION__,"传入参数strSql2p = 【{0}】",(const char*)strSql2p);
			/*strSql2p += " AND DEST_FIN = @dest_fin" ;*/
		}
		//新增材料状态下拉框
		if (v_mat_status.Trim().GetLength() > 0)
		{
            l_max = (v_mat_status.Trim().GetLength()+2)/4;
			Log::Trace("",__FUNCTION__,"v_mat_status.l_max = 【{0}】len[{1}]",l_max,v_mat_status.Trim().GetLength());
            for(int l = 0;l<l_max;l++)
            {
				if(l == 0)
				{
					strSql2p = strSql2p + " AND a.MAT_STATUS in ('" + v_mat_status.Trim().Substring(l*4,2) + "'";
				}
				else
				{
					strSql2p += ",'" + v_mat_status.Trim().Substring(l*4,2) + "'";
				}
            }
            strSql2p = strSql2p + ") ";
			Log::Trace("", __FUNCTION__, "传入参数strSql2p = 【{0}】", (const char*)strSql2p);
		}
		if (v_band_ord_sort.Trim() > "" && v_band_ord_sort.Trim() == "1")
		{
			strSql2p += " AND b.BAND_ORD_SORT = '1' " ;
		}
		else if (v_band_ord_sort.Trim() > "" && v_band_ord_sort.Trim() == "0")
		{
			strSql2p += " AND b.BAND_ORD_SORT != '1' " ;
		}
		if (secut_flag.Trim().Compare("1")==0)
		{
			strSql2p += " AND a.SECUT_FLAG = @secut_flag1 " ;
		}
		else if (secut_flag.Trim().Compare("2")==0)
		{
			strSql2p += " AND a.SECUT_FLAG BETWEEN @secut_flag1 AND @secut_flag2 " ;
		}
		 
		if (v_backlog56.Trim().GetLength() > 0)
		{
			strSql2p += " AND SUBSTR(a.BACKLOG,5,2) like @backlog1  " ;
		}
		if (v_backlog78.Trim().GetLength() > 0)
		{
			strSql2p += " AND SUBSTR(a.BACKLOG,7,2) like @backlog2  " ;
		}
		if (v_backlog9a.Trim().GetLength() > 0)
		{
			strSql2p += " AND SUBSTR(a.BACKLOG,9,2) like @backlog3  " ;
		}
		if (v_backlogdf.Trim().GetLength() > 0)
		{
			strSql2p += " AND SUBSTR(a.BACKLOG,13,1) like @backlog4  " ;
			strSql2p += " AND SUBSTR(a.BACKLOG,15,1) like @backlog5  " ;
		}
		if (st_no_flag.Trim()=="1")
		{
			strSql2p += " AND (a.ST_NO IN (SELECT ST_NO FROM TMMSM0L80) OR SUBSTR(ST_NO,1,1)||'XXXXXXX' IN (SELECT ST_NO FROM TMMSM0L80))  " ;
		}
		if (product_flag.Trim()=="1"||product_flag.Trim()=="0")
		{
			strSql2p += " AND a.PRODUCT_FLAG like @product_flag " ;
		}
		
		
		if (mat_group.Trim().GetLength() > 0)
		{
			strSql2p += " AND a.MAT_GROUP like @mat_group " ;
		}
		if (quality_grade.Trim().GetLength() > 0)
		{
			strSql2p += " AND a.QUALITY_GRADE like @quality_grade " ;
		}
		//20130828选中炉次主档说明这些材料是不在炉次主档中的
		if(v_query_is1f == "0")
		{
			strSql2p += " AND a.PONO NOT IN (SELECT  PONO FROM TMMSMIS1F  ORDER BY PONO ) ";
		}
		Log::Trace("",__FUNCTION__,"strSql2paaaa = 【{0}】",(const char*)strSql2p);
		//20140912选中说明是未清理的材料，材料原因代码1的第二位是1、2、3
		if(v_clean_if_flag == "0")
		{
			strSql2p += " AND  trim(a.MAT_CAUSE_CODE1) <> '5E'  ";
			strSql2p += " AND (trim(a.MAT_CAUSE_CODE1) > '' AND SUBSTR(a.MAT_CAUSE_CODE1,2,1) in ('1','2','3')) ";
		}
		Log::Trace("",__FUNCTION__,"strSql2pTTT = 【{0}】",(const char*)strSql2p);

		Log::Trace("",__FUNCTION__,"ibb_flag = 【{0}】",(const char*)ibb_flag);
		//20150821选中IBB说明这些材料在外购材料界面档（TMM00WG01）中的FACTORY_ID为IBB
		if(ibb_flag == "1")
		{
			strSql2p += " AND a.MAT_NO IN (SELECT  MAT_NO  FROM TMM00WG01 WHERE  MAT_NO = a.MAT_NO  AND RAW_ORIGIN = '051035' AND FACTORY_ID = 'IBB') ";
			strSql2p += " AND a.RAW_ORIGIN = '051035' ";
		}
		Log::Trace("",__FUNCTION__,"strSql2pTTp = 【{0}】",(const char*)strSql2p);

		//根据静态表“限时热送钢种维护”（PSSM28）中的钢种对在库板坯根据出钢记号进行筛选，并将静态表中“热装方式”字段带过来并显示
		Log::Trace("",__FUNCTION__,"hot_flag = 【{0}】",(const char*)hot_flag);
		if(hot_flag == "1")
		{
			strSql2p += " AND a.ST_NO IN (SELECT  ST_NO  FROM PSSM.TPSSM28 WHERE  ST_NO = a.ST_NO  ) ";
		}
		Log::Trace("",__FUNCTION__,"strSql2pTTf = 【{0}】",(const char*)strSql2p);

		Log::Trace("",__FUNCTION__,"hot_test_code = 【{0}】",(const char*)hot_test_code);
		//20160224新增热送试验代码
		if (hot_test_code.Trim().GetLength() > 0)
		{
			strSql2p += " AND a.HOT_TEST_CODE like @hot_test_code " ;
		}

		//20170302新增预匹配标记
		if (form_plate_flag.Trim() == "有")
		{
			strSql2p += " AND trim(a.FORM_PLATE_FLAG) > ' ' AND  trim(a.FORM_PLATE_FLAG) != '0'  ";
		}
		else if (form_plate_flag.Trim() == "无")
		{
			strSql2p += " AND trim(a.FORM_PLATE_FLAG) <= ' ' ";
		}
		Log::Trace("", __FUNCTION__, "form_plate_flag1 = 【{0}】", (const char*)form_plate_flag);
		Log::Trace("", __FUNCTION__, "strSql2pQ = 【{0}】", (const char*)strSql2p);

		//20170322新增二切计划标记
		if (secut_plan_no.Trim() == "有")
		{
			strSql2p += " AND trim(a.SECUT_PLAN_NO) > ' ' ";
		}
		else if (secut_plan_no.Trim() == "无")
		{
			strSql2p += " AND trim(a.SECUT_PLAN_NO) <= ' ' ";
		}

		
		//20140703新增炉号的判断（多笔）
		if(pono.Trim() != "")
		{
			strSql2p += " AND ( ";
			for(int j=0;j<row_count;j++)
			{
				v_pono = bcls_rec->Tables[0].Rows[j]["pono"].ToString().Trim();
				if(v_pono.Trim() != "")
				{
					if(j==0)
					{
						strSql2p += " a.PONO like  '"+v_pono+"'"; 
					}
					else
					{
						strSql2p += " or a.PONO like  '"+v_pono+"'"; 
					}
				}
			}
			strSql2p += " )";
		}
		Log::Trace("",__FUNCTION__,"strSql2pn = 【{0}】",(const char*)strSql2p);

		/*根据标记查询在线档或历史档  分页*/
		if(v_query_flag == "0")
		{
			strSql2 = "SELECT COUNT(a.MAT_NO) FROM TMMSM01 a left join OMPO.TOM01 b  on a.ORDER_NO = b.ORDER_NO where 1 = 1 ";
		}
		else if(v_query_flag == "1")
		{
			strSql2 = "SELECT COUNT(a.MAT_NO) FROM HMMSM01 a left join OMPO.TOM01 b  on a.ORDER_NO = b.ORDER_NO where 1 = 1 ";
		}

		strSql2 = strSql2 + strSql2p;
		Log::Trace("",__FUNCTION__,"strSql2 = 【{0}】",(const char*)strSql2);

		cmdcount.SetCommandText(strSql2);
		/*if (v_pono.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("pono",v_pono);
		}
		else
		{
		    cmdcount.Parameters.Set("pono","%");
		}*/
		/*if (v_mat_status.Trim().GetLength() > 0 && v_mat_status.Trim().GetLength() <= 2)
		{
			cmdcount.Parameters.Set("mat_status",v_mat_status);
		}
		else if(v_mat_status.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("mat_status1","11");
			cmdcount.Parameters.Set("mat_status2","12");
		}*/

		
		if(orderNo.Trim().GetLength() > 0 && orderNo.Trim() != "N" && orderNo.Trim() != "Y" )
		{
			cmdcount.Parameters.Set("order_no",orderNo);
		}

		if(whole_backlog_code1.Trim().GetLength() > 0  &&  v_whole_backlog_code1.Trim().GetLength() > 0)
		{
			/*cmdcount.Parameters.Set("whole_backlog_code1","%" + whole_backlog_code1 + "%");*/
			cmdcount.Parameters.Set("whole_backlog_code1",whole_backlog_code1);
			cmdcount.Parameters.Set("v_whole_backlog_code1",v_whole_backlog_code1);
		}

		if(whole_backlog_code2.Trim().GetLength() > 0  &&  v_whole_backlog_code2.Trim().GetLength() > 0)
		{
			/*cmdcount.Parameters.Set("whole_backlog_code2","%" + whole_backlog_code2 + "%");*/
			cmdcount.Parameters.Set("whole_backlog_code2",whole_backlog_code2);
			cmdcount.Parameters.Set("v_whole_backlog_code2",v_whole_backlog_code2);
		}

		if(whole_backlog_code3.Trim().GetLength() > 0 &&  v_whole_backlog_code3.Trim().GetLength() > 0 )
		{
			/*cmdcount.Parameters.Set("whole_backlog_code3","%" + whole_backlog_code3 + "%");*/
			cmdcount.Parameters.Set("whole_backlog_code3",whole_backlog_code3);
			cmdcount.Parameters.Set("v_whole_backlog_code3",v_whole_backlog_code3);
		}

		
		if (v_stock_no.Trim().GetLength() > 0)
		{
		    if(v_stock_no.Trim().GetLength() < 5)
		    {
		        cmdcount.Parameters.Set("stock_no",v_stock_no);
		    }
		    else
		    {
		        L_N =0;
		        l_max = bcls_rec->Tables["STOCK_NO_IN"].Rows.get_Count();
			    for(int l =0;l<l_max;l++)
			    {
				    i_stock_no = "stock_no" + L_N.ToString();
				    cmdcount.Parameters.Set(i_stock_no ,bcls_rec->Tables["STOCK_NO_IN"].Rows[l]["STOCK_NO"]);
				    L_N = L_N + 1;
			    }
			}
		}
		
		
		if (v_st_no.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("v_st_no",v_st_no);
		}

		if (v_store_area.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("v_store_area",v_store_area);
		}
		
		if (v_direct.Trim().GetLength() > 0)
		{
            L_N =0;
            l_max = bcls_rec->Tables["DEST"].Rows.get_Count();
            for(int l =0;l<l_max;l++)
            {
                i_dest_fin = "dest_fin" + L_N.ToString();
                cmdcount.Parameters.Set(i_dest_fin ,bcls_rec->Tables["DEST"].Rows[l]["DEST_FIN"]);
                L_N = L_N + 1;
            }
			
		}
		if (v_ai_time_1_f.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("ai_time_1_fr",v_ai_time_1_f);
		}
		
		if (v_ai_time_1_t.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("ai_time_1_to",v_ai_time_1_t);
		}
	
		if (secut_flag.Trim().Compare("1")==0)
		{
			cmdcount.Parameters.Set("secut_flag1"," ");		
		}
		else if (secut_flag.Trim().Compare("2")==0)
		{
			cmdcount.Parameters.Set("secut_flag1","1");
			cmdcount.Parameters.Set("secut_flag2","2");
		}
		if (v_backlog56.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("backlog1",v_backlog56);	
		}
		if (v_backlog78.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("backlog2",v_backlog78);
		}
		if (v_backlog9a.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("backlog3",v_backlog9a);
		}
		if (v_backlogdf.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("backlog4",v_backlogdf.Substring(0,1));
			cmdcount.Parameters.Set("backlog5",v_backlogdf.Substring(1,1));
		}
		if (product_flag.Trim()=="1"||product_flag.Trim()=="0")
		{
			cmdcount.Parameters.Set("product_flag",product_flag);
		}
		if (hot_test_code.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("hot_test_code",hot_test_code);
		}
		if (mat_group.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("mat_group",mat_group);
		}
		if (quality_grade.Trim().GetLength() > 0)
		{
			cmdcount.Parameters.Set("quality_grade",quality_grade);
		}
		
		/*此方法返回单行单列数据*/
		Log::Trace("",__FUNCTION__,"v_whole_backlog_code = 【{0}】",v_whole_backlog_code1);
		Log::Trace("",__FUNCTION__,"strSql2pTTK = 【{0}】",(const char*)strSql2p);
		CDecimal nRecCount = cmdcount.ExecuteScalar();
		
		Log::Trace("",__FUNCTION__,"nRecCount = 【{0}】",nRecCount);
		Log::Trace("",__FUNCTION__,"anRecCount = 【{0}】",nRecCount);
		/*返回记录数，分页显示用*/
		bcls_ret->ExtendedProperties.Add("REC_COUNT", nRecCount.ToString());
		/*根据标记查询在线档或历史档*/
		Log::Trace("",__FUNCTION__,"v_query_flag = 【{0}】",(const char*)v_query_flag);
		if (v_query_flag == "0")
		{
			strSql2 = "SELECT a.MAT_NO,a.ST_NO,a.MAT_ACT_THICK,a.MAT_ACT_WIDTH,a.MAT_ACT_LEN,a.MAT_ACT_WT,a.WHOLE_BACKLOG_CODE,a.MAT_GROUP,a.QUALITY_GRADE,a.HSF_END_TIME,a.MAT_MATCH_ERR_CODE,"
			          "a.DEST_FIN,a.STOCK_NO,a.MAT_STATUS,a.SLAB_CUT_TIME,a.ORDER_NO,a.BATCH_PROD_CODE,b.ORDER_STATUS,b.DELIVY_WEEK_FLAG,b.ORDER_DELIVERY_DATE,b.ORDER_TYPE_CODE,a.SLAT_UNLADE_CAUSE,"
			          "a.PLAN_NO,a.CONFM_PLAN_NO,a.TRANSFER_PLAN_NO,a.BACKLOG, ' ' as flag,a.MNG_HOLD_CAUSE_CODE,b.BAND_ORD_SORT,a.SLAB_REMARK,a.WHOLE_BACKLOG,a.WIDTH_TOP_SLAB,a.WIDTH_BOT_SLAB, "
					  "a.IN_STOCK_HOT_TIME,a.OUT_STOCK_TIME,a.PONO,a.ST_CHE_CAUSE_CODE,a.MAT_CAUSE_CODE1,a.HOT_TEST_CODE,a.SLAB_PLACE_CODE,a.STORE_AREA,a.FORM_PLATE_FLAG,a.SECUT_PLAN_NO,a.STORE_AREA "
			          " FROM TMMSM01 a left join OMPO.TOM01 b  on a.ORDER_NO = b.ORDER_NO where 1 = 1 ";
			strSql2 += strSql2p;
		}
		else if (v_query_flag == "1")
		{
		    strSql2 = "SELECT a.MAT_NO,a.ST_NO,a.MAT_ACT_THICK,a.MAT_ACT_WIDTH,a.MAT_ACT_LEN,a.MAT_ACT_WT,a.WHOLE_BACKLOG_CODE,a.MAT_GROUP,a.QUALITY_GRADE,a.HSF_END_TIME,a.MAT_MATCH_ERR_CODE,"
			          "a.DEST_FIN,a.STOCK_NO,a.MAT_STATUS,a.SLAB_CUT_TIME,a.ORDER_NO,a.BATCH_PROD_CODE,b.ORDER_STATUS,b.DELIVY_WEEK_FLAG,b.ORDER_DELIVERY_DATE,b.ORDER_TYPE_CODE,a.SLAT_UNLADE_CAUSE, "
			          "a.PLAN_NO,a.CONFM_PLAN_NO,a.TRANSFER_PLAN_NO,a.BACKLOG, ' ' as flag,a.MNG_HOLD_CAUSE_CODE,b.BAND_ORD_SORT,a.SLAB_REMARK,a.WHOLE_BACKLOG,a.WIDTH_TOP_SLAB,a.WIDTH_BOT_SLAB, "
					  "a.IN_STOCK_HOT_TIME,a.OUT_STOCK_TIME,a.PONO,a.ST_CHE_CAUSE_CODE,a.MAT_CAUSE_CODE1,a.HOT_TEST_CODE,a.SLAB_PLACE_CODE,a.STORE_AREA,a.FORM_PLATE_FLAG,a.SECUT_PLAN_NO,a.STORE_AREA "
			          " FROM HMMSM01 a left join OMPO.TOM01 b  on a.ORDER_NO = b.ORDER_NO where 1 = 1 ";
			strSql2 += strSql2p;
		}
		
	    strSql2 = strSql2 + orderby_sql ;
		
		Log::Trace("",__FUNCTION__,"strSql2B = 【{0}】",(const char*)strSql2);

		cmdinq.SetCommandText(strSql2);
		cmdinq.Parameters.Clear();
		if(v_query_flag == "0" ||v_query_flag == "1")
		{
		     
			/*if (v_pono.Trim().GetLength() > 0)
			{
				cmdinq.Parameters.Set("pono",v_pono);
			}*/
		
			/*if (v_mat_status.Trim().GetLength() > 0 && v_mat_status.Trim().GetLength() <= 2)
			{
				cmdinq.Parameters.Set("mat_status",v_mat_status);
				Log::Trace("",__FUNCTION__,"v_mat_status = 【{0}】",(const char*)v_mat_status);
			}
			else if(v_mat_status.Trim().GetLength() > 0)
			{
				cmdinq.Parameters.Set("mat_status1","11");
				cmdinq.Parameters.Set("mat_status2","12");
			}*/
			if(orderNo.Trim().GetLength() > 0 && orderNo.Trim() != "N" && orderNo.Trim() != "Y" )
			{
				cmdinq.Parameters.Set("order_no",orderNo);
			}
			
			if(whole_backlog_code1.Trim().GetLength() > 0  &&  v_whole_backlog_code1.Trim().GetLength() > 0)
			{
				/*cmdinq.Parameters.Set("whole_backlog_code1","%" + whole_backlog_code1 + "%");*/
				cmdinq.Parameters.Set("whole_backlog_code1",whole_backlog_code1);
				cmdinq.Parameters.Set("v_whole_backlog_code1",v_whole_backlog_code1);
			}

			if(whole_backlog_code2.Trim().GetLength() > 0 &&  v_whole_backlog_code2.Trim().GetLength() > 0)
			{
				/*cmdinq.Parameters.Set("whole_backlog_code2","%" + whole_backlog_code2 + "%");*/
				cmdinq.Parameters.Set("whole_backlog_code2",whole_backlog_code2);
				cmdinq.Parameters.Set("v_whole_backlog_code2",v_whole_backlog_code2);
			}

			if(whole_backlog_code3.Trim().GetLength() > 0  &&  v_whole_backlog_code3.Trim().GetLength() > 0 )
			{
				/*cmdinq.Parameters.Set("whole_backlog_code3","%" + whole_backlog_code3 + "%");*/
				cmdinq.Parameters.Set("whole_backlog_code3",whole_backlog_code3);
				cmdinq.Parameters.Set("v_whole_backlog_code3",v_whole_backlog_code3);
			}
			 
			if (v_stock_no.Trim().GetLength() > 0)
			{
			    if(v_stock_no.Trim().GetLength() < 5)
		        {
		            cmdinq.Parameters.Set("stock_no",v_stock_no);
		        }
		        else
		        {
				    L_N =0;
		            l_max = bcls_rec->Tables["STOCK_NO_IN"].Rows.get_Count();
			        for(int l =0;l<l_max;l++)
			        {
				        i_stock_no = "stock_no" + L_N.ToString();
				        cmdinq.Parameters.Set(i_stock_no ,bcls_rec->Tables["STOCK_NO_IN"].Rows[l]["STOCK_NO"]);
				        L_N = L_N + 1;
			        }
			    }
			}
			
			if (v_st_no.Trim().GetLength() > 0)
			{
				cmdinq.Parameters.Set("v_st_no",v_st_no);
			}
			if (v_store_area.Trim().GetLength() > 0)
			{
				cmdinq.Parameters.Set("v_store_area", v_store_area);
			}
			
			if (v_direct.Trim().GetLength() > 0)
			{
				L_N =0;
                l_max = bcls_rec->Tables["DEST"].Rows.get_Count();
                for(int l =0;l<l_max;l++)
                {
                    i_dest_fin = "dest_fin" + L_N.ToString();
                    cmdinq.Parameters.Set(i_dest_fin ,bcls_rec->Tables["DEST"].Rows[l]["DEST_FIN"]);
                    L_N = L_N + 1;
                    
                }
			}
			
			if (v_ai_time_1_f.Trim().GetLength() > 0)
			{
				cmdinq.Parameters.Set("ai_time_1_fr",v_ai_time_1_f);
			}
			
			
			if (v_ai_time_1_t.Trim().GetLength() > 0)
			{
				cmdinq.Parameters.Set("ai_time_1_to",v_ai_time_1_t);
			}
			
			
			if (secut_flag.Trim().Compare("1")==0)
			{
				cmdinq.Parameters.Set("secut_flag1"," ");	
			}
			else if (secut_flag.Trim().Compare("2")==0)
			{
				cmdinq.Parameters.Set("secut_flag1","1");
				cmdinq.Parameters.Set("secut_flag2","2");
			}
			if (v_backlog56.Trim().GetLength() > 0)
		    {
			    cmdinq.Parameters.Set("backlog1",v_backlog56);	
		    }
		    if (v_backlog78.Trim().GetLength() > 0)
		    {
			    cmdinq.Parameters.Set("backlog2",v_backlog78);
		    }
		    if (v_backlog9a.Trim().GetLength() > 0)
		    {
			    cmdinq.Parameters.Set("backlog3",v_backlog9a);
		    }
		    if (v_backlogdf.Trim().GetLength() > 0)
		    {
			    cmdinq.Parameters.Set("backlog4",v_backlogdf.Substring(0,1));
			    cmdinq.Parameters.Set("backlog5",v_backlogdf.Substring(1,1));
		    }
			if (product_flag.Trim()=="1"||product_flag.Trim()=="0")
			{
				cmdinq.Parameters.Set("product_flag",product_flag);
			}

			if (hot_test_code.Trim().GetLength() > 0)
			{
				cmdinq.Parameters.Set("hot_test_code",hot_test_code);
			}
			if (mat_group.Trim().GetLength() > 0)
			{
				cmdinq.Parameters.Set("mat_group",mat_group);
			}
			if (quality_grade.Trim().GetLength() > 0)
			{
				cmdinq.Parameters.Set("quality_grade",quality_grade);
			}
			
		}
		
		/*blkNum=bcls_ret->Tables.IndexOf("TMMSMIS1F");*/ 
		//f_edsetcustominfo(bcls_rec, bcls_ret, conn);
		/*此方法是直接压入block数据块，支持分页查询*/
		cmdinq.ExecuteQuery(bcls_ret->Tables["MMSMIS02A1_INQ"],nPageStart ,nPageSize);
		//bcls_ret->Tables["MMSMIS02_INQ"].Columns.Add(DT_STRING,"DELIVY_WEEK_FLAG");
        //bcls_ret->Tables["MMSMIS02_INQ"].Columns.Add(DT_STRING,"ORDER_DELIVERY_DATE");
        bcls_ret->Tables["MMSMIS02A1_INQ"].Columns.Add(DT_STRING,"BACKLOG56");
        bcls_ret->Tables["MMSMIS02A1_INQ"].Columns.Add(DT_STRING,"BACKLOG78");
        bcls_ret->Tables["MMSMIS02A1_INQ"].Columns.Add(DT_STRING,"BACKLOG9A");
        bcls_ret->Tables["MMSMIS02A1_INQ"].Columns.Add(DT_STRING,"BACKLOGDF");
		//20160219新增列热装方式,产出时间（小时）、在库时间（小时）
		bcls_ret->Tables["MMSMIS02A1_INQ"].Columns.Add(DT_STRING,"HOT_CHARGE_METHOD");
		bcls_ret->Tables["MMSMIS02A1_INQ"].Columns.Add(DT_STRING,"IN_STOCK_DURA");
		bcls_ret->Tables["MMSMIS02A1_INQ"].Columns.Add(DT_STRING,"IN_STOCK_TIME_DURA");
		bcls_ret->Tables["MMSMIS02A1_INQ"].Columns.Add(DT_STRING,"THAN_TIME");

		//20170421新增催货日期
		bcls_ret->Tables["MMSMIS02A1_INQ"].Columns.Add(DT_STRING, "CONFM_PRG_PLAN_DATE"); //催货日期

		
		for(int row = 0;row<bcls_ret->Tables["MMSMIS02A1_INQ"].Rows.get_Count();row++)
		{
		    v_order_no = bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["ORDER_NO"];
		    v_backlog = bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["BACKLOG"];
		    v_dest_fin = bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["DEST_FIN"];
		    b_band_ord_sort = bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["BAND_ORD_SORT"];
			v_st_no_temp = bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["ST_NO"];
			v_in_stock_hot_time = bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["IN_STOCK_HOT_TIME"];
			v_slab_cut_time = bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["SLAB_CUT_TIME"];

			//20170421新增按周交货标记
			delivy_week_flag = bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["DELIVY_WEEK_FLAG"];

			//20170421新增催货日期查询(按周交货标志为D的合同)
			if (delivy_week_flag.Trim().GetLength()> 0 &&
				delivy_week_flag.Trim() == 'D' &&
				v_order_no.Trim().GetLength() > 0)
			{
				confm_prg_plan_date = "";
				sqlstrt1 = "SELECT CONFM_PRG_PLAN_DATE FROM PMOF.TPMOFA18 WHERE ORDER_NO = @v_order_no ORDER BY SERIAL_NO DESC FETCH FIRST 1 ROWS ONLY ";
				com.SetCommandText(sqlstrt1);
				com.Parameters.Clear();
				com.Parameters.Set("v_order_no", v_order_no);
				com.ExecuteReader();
				if (com.Read())
				{
					confm_prg_plan_date = com.GetString(1);
				}
				else
				{
					confm_prg_plan_date = "";
				}
				com.Close();
				Log::Trace("", __FUNCTION__, "confm_prg_plan_date = [{0}]", (const char*)confm_prg_plan_date);
				bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["CONFM_PRG_PLAN_DATE"] = confm_prg_plan_date;
			}

		    if(v_backlog.Trim().GetLength()>16)
		    {
		        backlog_tmp = ""; 
                if(v_dest_fin == "05"||v_dest_fin == "06")
                {
                    sqlstr = "SELECT unit_code "
                        " FROM SIPM.TSIPMOF02  WHERE BACKLOG_VAL = @backlog and BACKLOG_POS = @backlog_pos"	;
                      
                }
                else
                {
                    sqlstr = "SELECT unit_code "
                        " FROM SIPM.TSIPMOF01 WHERE BACKLOG_VAL = @backlog and BACKLOG_POS = @backlog_pos ";
                }

                cmdinq.SetCommandText(sqlstr);
                cmdinq.Parameters.Clear();
                cmdinq.Parameters.Set("backlog",v_backlog.Substring(4,1));
                cmdinq.Parameters.Set("backlog_pos","05");
                cmdinq.ExecuteReader();
                if(cmdinq.Read())
                {
                    backlog_tmp += cmdinq.GetString(1);
                }
                cmdinq.Close();
                
                cmdinq.Parameters.Clear();
                cmdinq.Parameters.Set("backlog",v_backlog.Substring(5,1));
                cmdinq.Parameters.Set("backlog_pos","06");
                cmdinq.ExecuteReader();
                if(cmdinq.Read())
                {
                    if(backlog_tmp.Trim() <= "")
					{
						backlog_tmp = cmdinq.GetString(1);
					}
					else
					{
						backlog_tmp =  backlog_tmp + "-" + cmdinq.GetString(1);
					}
                }
                cmdinq.Close();
                
                bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["BACKLOG56"] = backlog_tmp;
              
                backlog_tmp = "";
                cmdinq.Parameters.Clear();
                cmdinq.Parameters.Set("backlog",v_backlog.Substring(6,1));
                cmdinq.Parameters.Set("backlog_pos","07");
                cmdinq.ExecuteReader();
                if(cmdinq.Read())
                {
                    backlog_tmp += cmdinq.GetString(1);
                }
                cmdinq.Close();
                
                cmdinq.Parameters.Clear();
                cmdinq.Parameters.Set("backlog",v_backlog.Substring(7,1));
                cmdinq.Parameters.Set("backlog_pos","08");
                cmdinq.ExecuteReader();
                if(cmdinq.Read())
                {
                    if(backlog_tmp.Trim() <= "")
					{
						backlog_tmp = cmdinq.GetString(1);
					}
					else
					{
						backlog_tmp =  backlog_tmp + "-" + cmdinq.GetString(1);
					}
                }
                cmdinq.Close();
                
				if(v_backlog.Substring(6,2)=="00"&&b_band_ord_sort.Trim() =="1")
				{
					backlog_tmp = "C402";
				}
				if(v_backlog.Substring(6,2)=="00"&&b_band_ord_sort.Trim() !="1")
				{
					backlog_tmp = "H000";
				}
                bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["BACKLOG78"] = backlog_tmp;
				
                
                
                backlog_tmp = "";
                cmdinq.Parameters.Clear();
                cmdinq.Parameters.Set("backlog",v_backlog.Substring(8,1));
                cmdinq.Parameters.Set("backlog_pos","09");
                cmdinq.ExecuteReader();
                if(cmdinq.Read())
                {
                    backlog_tmp += cmdinq.GetString(1);
                }
                cmdinq.Close();
                
                cmdinq.Parameters.Clear();
                cmdinq.Parameters.Set("backlog",v_backlog.Substring(9,1));
                cmdinq.Parameters.Set("backlog_pos","10");
                cmdinq.ExecuteReader();
                if(cmdinq.Read())
                {
                    if(backlog_tmp.Trim() <= "")
					{
						backlog_tmp = cmdinq.GetString(1);
					}
					else
					{
						backlog_tmp =  backlog_tmp + "-" + cmdinq.GetString(1);
					}
                }
                cmdinq.Close();
                
                bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["BACKLOG9A"] = backlog_tmp;
                
                backlog_tmp = "";
                cmdinq.Parameters.Clear();
                cmdinq.Parameters.Set("backlog",v_backlog.Substring(12,1));
                cmdinq.Parameters.Set("backlog_pos","13");
                cmdinq.ExecuteReader();
                if(cmdinq.Read())
                {
                    backlog_tmp += cmdinq.GetString(1);
                }
                cmdinq.Close();
                
                cmdinq.Parameters.Clear();
                cmdinq.Parameters.Set("backlog",v_backlog.Substring(14,1));
                cmdinq.Parameters.Set("backlog_pos","15");
                cmdinq.ExecuteReader();
                if(cmdinq.Read())
                {
                    if(backlog_tmp.Trim() <= "")
					{
						backlog_tmp = cmdinq.GetString(1);
					}
					else
					{
						backlog_tmp =  backlog_tmp + "-" + cmdinq.GetString(1);
					}
                }
                cmdinq.Close();
                bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["BACKLOGDF"] = backlog_tmp;
				

			}
			if(v_st_no_temp.Trim().GetLength()>0)
			{
				//产出时间计算
				//CDecimal hourDiff = GetHourDiff(CDateTime::Now().ToString("yyyyMMddHHmmss"), v_slab_cut_time);
				//CDecimal hourDiff = TIMESTAMPDIFF(8,CHAR(CURRENT TIMESTAMP-TO_DATE(v_slab_cut_time,'YYYYMMDDHH24MISS')));
				if(v_slab_cut_time.Trim().GetLength() == 14)
				{
// DM8 适配 CHANGE-165:查询当前时间减切断时间的小时数,写入 IN_STOCK_DURA(在库时长)。
// 改写原因：DB2 两参数 TIMESTAMPDIFF(8=小时,当前时间减切断时间的小时数,写入 IN_STOCK_DURA 在库时长) 在 DM8 无对应写法,按 HR-002 确认口径①(实际完整时长)
//   改为 DATEDIFF(SECOND,起点=切断时间,终点=当前时间)/3600,整数除法与 DB2 截断行为一致。
//   timestamp(x,格式串) 改为 TO_TIMESTAMP(x,'YYYYMMDDHH24MISS'):时间为 14 位值(HR-001 已确认),原格式串 yyyy-MM-DD hh:mm:ss 与值不匹配;
//   当前时点沿用 CURRENT_TIMESTAMP(DM 官方函数手册支持);SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
					// sqlstr_COLDTIME="select timestampdiff(8,char(current timestamp - timestamp('"+v_slab_cut_time+"','yyyy-MM-DD hh:mm:ss')))  from sysibm.sysdummy1";
// DM8 SQL：
					sqlstr_COLDTIME="select DATEDIFF(SECOND, TO_TIMESTAMP('"+v_slab_cut_time+"','YYYYMMDDHH24MISS'), CURRENT_TIMESTAMP) / 3600 from DUAL";
					comma.SetCommandText(sqlstr_COLDTIME);
					comma.ExecuteReader();
					if(comma.Read())
					{
						CDecimal in_stock_dura = comma.GetDecimal(1);  //在库时间
						if(in_stock_dura > 999999)
						{
							in_stock_dura = 999999;
						}
						//Log::Trace("",__FUNCTION__,"in_stock_dura b= [{0}]", (const char*)in_stock_dura);
						bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["IN_STOCK_DURA"] = in_stock_dura; 
					}
					comma.Close();
					if(v_in_stock_hot_time.Trim().GetLength() == 14)
					{
// DM8 适配 CHANGE-166:查询切断到热装入库的小时数,写入 IN_STOCK_TIME_DURA。
// 改写原因：DB2 两参数 TIMESTAMPDIFF(8=小时,切断到热装入库的小时数,写入 IN_STOCK_TIME_DURA) 在 DM8 无对应写法,按 HR-002 确认口径①(实际完整时长)
//   改为 DATEDIFF(SECOND,起点=切断时间,终点=热装入库时间)/3600,整数除法与 DB2 截断行为一致。
//   timestamp(x,格式串) 改为 TO_TIMESTAMP(x,'YYYYMMDDHH24MISS'):时间为 14 位值(HR-001 已确认),原格式串 yyyy-MM-DD hh:mm:ss 与值不匹配;
//   SYSIBM 辅助表改为 DUAL;依据 DM 官方文档,DM8 尚未实测。
// 本共用分支面向 DM8,其他 DB_KIND 标签也会执行此 SQL;参数、结果列、条件与排序保持不变。
// 原 SQL（完整保留）：
						// sqlstr_COLDTIME="select timestampdiff(8,char(timestamp('"+v_in_stock_hot_time+"','yyyy-MM-DD hh:mm:ss') - timestamp('"+v_slab_cut_time+"','yyyy-MM-DD hh:mm:ss')))  from sysibm.sysdummy1";
// DM8 SQL：
						sqlstr_COLDTIME="select DATEDIFF(SECOND, TO_TIMESTAMP('"+v_slab_cut_time+"','YYYYMMDDHH24MISS'), TO_TIMESTAMP('"+v_in_stock_hot_time+"','YYYYMMDDHH24MISS')) / 3600 from DUAL";
						comma.SetCommandText(sqlstr_COLDTIME);
						comma.ExecuteReader();
						if(comma.Read())
						{
						    in_stock_time_dura = comma.GetDecimal(1);  //在库时间
							if(in_stock_time_dura > 999999)
							{
								in_stock_time_dura = 999999;
							}
							//Log::Trace("",__FUNCTION__,"in_stock_time_dura b= [{0}]", (const char*)in_stock_time_dura);
							bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["IN_STOCK_TIME_DURA"] = in_stock_time_dura; 
						}
						comma.Close();
					}
				}

				//在库时间计算
				/*CDecimal hourDiff1 = GetHourDiff(v_in_stock_hot_time.ToString("yyyyMMddHHmmss"), v_slab_cut_time);
				bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["IN_STOCK_DURA"] = hourDiff; 
				bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["IN_STOCK_TIME_DURA"] = hourDiff1; */

				//限时热装钢种静态表 TPSSM28
				sqlstr = "SELECT HOT_CHARGE_METHOD "
					" FROM PSSM.TPSSM28 WHERE ST_NO = @v_st_no_temp "	;
				cmdinq.SetCommandText(sqlstr);
				cmdinq.Parameters.Clear();
				cmdinq.Parameters.Set("v_st_no_temp",v_st_no_temp);
				cmdinq.ExecuteReader();
				if(cmdinq.Read())
				{
					hot_charge_method_temp = cmdinq.GetString(1);
					if((hot_charge_method_temp == "L02R" && in_stock_time_dura > 2) ||(hot_charge_method_temp == "L04R" && in_stock_time_dura > 5 )
						||(hot_charge_method_temp == "L18R" && in_stock_time_dura > 18 ) ||(hot_charge_method_temp == "L56R" && in_stock_time_dura > 56 ))
					{
						bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["THAN_TIME"] = "是"; 
					}
					else
					{
						bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["THAN_TIME"] = " "; 
					}

				}
				else
				{
					hot_charge_method_temp = " ";
				}
                cmdinq.Close();
				bcls_ret->Tables["MMSMIS02A1_INQ"].Rows[row]["HOT_CHARGE_METHOD"] = hot_charge_method_temp; 
			}  

		}
		/*查询制造命令信息 传入块2*/
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
		
		bcls_ret->Tables[blkNum].Rows.Add();
		bcls_ret->Tables[blkNum].Rows[0]["PONO"] = " ";
		bcls_ret->Tables[blkNum].Rows[0]["CC_NUM"] = "0";
		bcls_ret->Tables[blkNum].Rows[0]["CAST_LOT_NO"] = " ";
		bcls_ret->Tables[blkNum].Rows[0]["HEAT_NO"] = " ";
		bcls_ret->Tables[blkNum].Rows[0]["ST_NO"] = " ";
		bcls_ret->Tables[blkNum].Rows[0]["CC_NO"] = " ";
		bcls_ret->Tables[blkNum].Rows[0]["CAST_NO"] = " ";
		bcls_ret->Tables[blkNum].Rows[0]["CAST_DIV_NO"] = " ";
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
		bcls_ret->Tables[blkNum].Rows[0]["TT_SLAB_WT"] = "0";			/*当前档材料总重*/
		bcls_ret->Tables[blkNum].Rows[0]["TT_SLAB_NUM"] = "0";			/*当前档材料总数*/
		bcls_ret->Tables[blkNum].Rows[0]["HT_SLAB_WT"] = "0";			/*历史档材料总重*/
		bcls_ret->Tables[blkNum].Rows[0]["HT_SLAB_NUM"] = "0";			/*历史档材料总数*/
	
		tt_slab_wt = 0;
		tt_slab_num = 0;
		ht_slab_wt = 0;
		ht_slab_num = 0;
		
		if (v_query_flag == "0")
	    {
		    //计算切断量
		    strSql1 = CString("SELECT SUM(a.MAT_ACT_WT),COUNT(a.MAT_NO) " 
			                  " FROM TMMSM01 a left join OMPO.TOM01 b  on a.ORDER_NO = b.ORDER_NO where 1 = 1 "
			                  );
			strSql1 = strSql1 + strSql2p;
		    cmdinq2.SetCommandText(strSql1);
		    /*if (v_pono.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("pono",v_pono);
		    }*/
			
			/*if (v_mat_status.Trim().GetLength() > 0 && v_mat_status.Trim().GetLength() <= 2)
			{
				cmdinq2.Parameters.Set("mat_status",v_mat_status);
			}
			else if(v_mat_status.Trim().GetLength() > 0)
			{
				cmdinq2.Parameters.Set("mat_status1","11");
				cmdinq2.Parameters.Set("mat_status2","12");
			}*/
			if(orderNo.Trim().GetLength() > 0 && orderNo.Trim() != "N" && orderNo.Trim() != "Y" )
			{
				cmdinq2.Parameters.Set("order_no",orderNo);
			}

			if(whole_backlog_code1.Trim().GetLength() > 0  &&  v_whole_backlog_code1.Trim().GetLength() > 0)
			{
				/*cmdinq2.Parameters.Set("whole_backlog_code1","%" + whole_backlog_code1 + "%");*/
				cmdinq2.Parameters.Set("whole_backlog_code1",whole_backlog_code1);
				cmdinq2.Parameters.Set("v_whole_backlog_code1",v_whole_backlog_code1);
			}

			if(whole_backlog_code2.Trim().GetLength() > 0 &&  v_whole_backlog_code2.Trim().GetLength() > 0 )
			{
				/*cmdinq2.Parameters.Set("whole_backlog_code2","%" + whole_backlog_code2 + "%");*/
				cmdinq2.Parameters.Set("whole_backlog_code2",whole_backlog_code2);
				cmdinq2.Parameters.Set("v_whole_backlog_code2",v_whole_backlog_code2);
			}

			if(whole_backlog_code3.Trim().GetLength() > 0  &&  v_whole_backlog_code3.Trim().GetLength() > 0)
			{
				/*cmdinq2.Parameters.Set("whole_backlog_code3","%" + whole_backlog_code3 + "%");*/
				cmdinq2.Parameters.Set("whole_backlog_code3",whole_backlog_code3);
				cmdinq2.Parameters.Set("v_whole_backlog_code3",v_whole_backlog_code3);
			}


		    if (v_stock_no.Trim().GetLength() > 0)
		    {
		        if(v_stock_no.Trim().GetLength() < 5)
		        {
		            v_stock_no = v_stock_no.Trim();
		            cmdinq2.Parameters.Set("stock_no",v_stock_no);
		        }
		        else
		        {
		            L_N =0;
		            l_max = bcls_rec->Tables["STOCK_NO_IN"].Rows.get_Count();
			        for(int l =0;l<l_max;l++)
			        {
				        i_stock_no = "stock_no" + L_N.ToString();
				        cmdinq2.Parameters.Set(i_stock_no ,bcls_rec->Tables["STOCK_NO_IN"].Rows[l]["STOCK_NO"]);
				        L_N = L_N + 1;
			        }
			    }
    			
    		
		    }
		    if (v_st_no.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("v_st_no",v_st_no);
		    }
			if (v_store_area.Trim().GetLength() > 0)
			{
				cmdinq2.Parameters.Set("v_store_area",v_store_area);
			}
		    if (v_direct.Trim().GetLength() > 0)
		    {
                L_N =0;
                l_max = bcls_rec->Tables["DEST"].Rows.get_Count();
                for(int l =0;l<l_max;l++)
                {
                    i_dest_fin = "dest_fin" + L_N.ToString();
                    cmdinq2.Parameters.Set(i_dest_fin ,bcls_rec->Tables["DEST"].Rows[l]["DEST_FIN"]);
                    L_N = L_N + 1;
                }
    			
		    }
		    if (v_ai_time_1_f.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("ai_time_1_fr",v_ai_time_1_f);
		    }
		    if (v_ai_time_1_t.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("ai_time_1_to",v_ai_time_1_t);
		    }
    		if (secut_flag.Trim().Compare("1")==0)
		    {
			    cmdinq2.Parameters.Set("secut_flag1"," ");		
		    }
		    else if (secut_flag.Trim().Compare("2")==0)
		    {
			    cmdinq2.Parameters.Set("secut_flag1","1");
			    cmdinq2.Parameters.Set("secut_flag2","2");
		    }
		    if (v_backlog56.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("backlog1",v_backlog56);	
		    }
		    if (v_backlog78.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("backlog2",v_backlog78);
		    }
		    if (v_backlog9a.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("backlog3",v_backlog9a);
		    }
		    if (v_backlogdf.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("backlog4",v_backlogdf.Substring(0,1));
			    cmdinq2.Parameters.Set("backlog5",v_backlogdf.Substring(1,1));
		    }
			if (product_flag.Trim()=="1"||product_flag.Trim()=="0")
			{
				cmdinq2.Parameters.Set("product_flag",product_flag);
			}
			if (hot_test_code.Trim().GetLength() > 0)
			{
				cmdinq2.Parameters.Set("hot_test_code",hot_test_code);
			}

			if (mat_group.Trim().GetLength() > 0)
			{
				cmdinq2.Parameters.Set("mat_group",mat_group);
			}
			if (quality_grade.Trim().GetLength() > 0)
			{
				cmdinq2.Parameters.Set("quality_grade",quality_grade);
			}
		
	        cmdinq2.ExecuteReader();
	        if(cmdinq2.Read())			/*cmdinq1*/
	        {
		        tt_slab_wt = cmdinq2.GetDecimal(1);
		        tt_slab_num = cmdinq2.GetDecimal(2);
	        }
	        cmdinq2.Close();
	    }
	    else if (v_query_flag == "1")
	    {
		    //计算历史切断量
		    strSql1 = CString("SELECT SUM(a.MAT_ACT_WT),COUNT(a.MAT_NO) " 
			                  " FROM HMMSM01 a left join OMPO.TOM01 b  on a.ORDER_NO = b.ORDER_NO  where 1 = 1 "
			                 );
			strSql1 = strSql1 + strSql2p;
		    cmdinq2.SetCommandText(strSql1);
		    /*if (v_pono.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("pono",v_pono);
		    }*/

			/*if (v_mat_status.Trim().GetLength() > 0 && v_mat_status.Trim().GetLength() <= 2)
			{
				cmdinq2.Parameters.Set("mat_status",v_mat_status);
			}
			else if(v_mat_status.Trim().GetLength() > 0)
			{
				cmdinq2.Parameters.Set("mat_status1","11");
				cmdinq2.Parameters.Set("mat_status2","12");
			}*/
			
			if(orderNo.Trim().GetLength() > 0 && orderNo.Trim() != "N" && orderNo.Trim() != "Y" )
			{
				cmdinq2.Parameters.Set("order_no",orderNo);
			}
			
			if(whole_backlog_code1.Trim().GetLength() > 0  &&  v_whole_backlog_code1.Trim().GetLength() > 0)
			{
				/*cmdinq2.Parameters.Set("whole_backlog_code1","%" + whole_backlog_code1 + "%");*/
				cmdinq2.Parameters.Set("whole_backlog_code1",whole_backlog_code1);
				cmdinq2.Parameters.Set("v_whole_backlog_code1",v_whole_backlog_code1);
			}

			if(whole_backlog_code2.Trim().GetLength() > 0  &&  v_whole_backlog_code2.Trim().GetLength() > 0 )
			{
				/*cmdinq2.Parameters.Set("whole_backlog_code2","%" + whole_backlog_code2 + "%");*/
				cmdinq2.Parameters.Set("whole_backlog_code2",whole_backlog_code2);
				cmdinq2.Parameters.Set("v_whole_backlog_code2",v_whole_backlog_code2);
			}

			if(whole_backlog_code3.Trim().GetLength() > 0   &&  v_whole_backlog_code3.Trim().GetLength() > 0 )
			{
				/*cmdinq2.Parameters.Set("whole_backlog_code3","%" + whole_backlog_code3 + "%");*/
				cmdinq2.Parameters.Set("whole_backlog_code3",whole_backlog_code3);
				cmdinq2.Parameters.Set("v_whole_backlog_code3",v_whole_backlog_code3);
			}

		    if (v_stock_no.Trim().GetLength() > 0)
		    {
		        if(v_stock_no.Trim().GetLength() < 5)
		        {
		            v_stock_no = v_stock_no.Trim() ;
		            cmdinq2.Parameters.Set("stock_no",v_stock_no);
		        }
		        else
		        {
		            L_N =0;
		            l_max = bcls_rec->Tables["STOCK_NO_IN"].Rows.get_Count();
			        for(int l =0;l<l_max;l++)
			        {
				        i_stock_no = "stock_no" + L_N.ToString();
				        cmdinq2.Parameters.Set(i_stock_no ,bcls_rec->Tables["STOCK_NO_IN"].Rows[l]["STOCK_NO"]);
				        L_N = L_N + 1;
			        }
			    }
    			
    		
		    }
		    if (v_st_no.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("v_st_no",v_st_no);
		    }
			if (v_store_area.Trim().GetLength() > 0)
			{
				cmdinq2.Parameters.Set("v_store_area",v_store_area);
			}
		    if (v_direct.Trim().GetLength() > 0)
		    {
                L_N =0;
                l_max = bcls_rec->Tables["DEST"].Rows.get_Count();
                for(int l =0;l<l_max;l++)
                {
                    i_dest_fin = "dest_fin" + L_N.ToString();
                    cmdinq2.Parameters.Set(i_dest_fin ,bcls_rec->Tables["DEST"].Rows[l]["DEST_FIN"]);
                    L_N = L_N + 1;
                }
    			
		    }
		    if (v_ai_time_1_f.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("ai_time_1_fr",v_ai_time_1_f);
		    }
		    if (v_ai_time_1_t.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("ai_time_1_to",v_ai_time_1_t);
		    }
    		if (secut_flag.Trim().Compare("1")==0)
		    {
			    cmdinq2.Parameters.Set("secut_flag1"," ");		
		    }
		    else if (secut_flag.Trim().Compare("2")==0)
		    {
			    cmdinq2.Parameters.Set("secut_flag1","1");
			    cmdinq2.Parameters.Set("secut_flag2","2");
		    }
		    if (v_backlog56.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("backlog1",v_backlog56);	
		    }
		    if (v_backlog78.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("backlog2",v_backlog78);
		    }
		    if (v_backlog9a.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("backlog3",v_backlog9a);
		    }
		    if (v_backlogdf.Trim().GetLength() > 0)
		    {
			    cmdinq2.Parameters.Set("backlog4",v_backlogdf.Substring(0,1));
			    cmdinq2.Parameters.Set("backlog5",v_backlogdf.Substring(1,1));
		    }
			if (product_flag.Trim()=="1"||product_flag.Trim()=="0")
			{
				cmdinq2.Parameters.Set("product_flag",product_flag);
			}

			if (hot_test_code.Trim().GetLength() > 0)
			{
				cmdinq2.Parameters.Set("hot_test_code",hot_test_code);
			}
			if (mat_group.Trim().GetLength() > 0)
			{
				cmdinq2.Parameters.Set("mat_group",mat_group);
			}
			if (quality_grade.Trim().GetLength() > 0)
			{
				cmdinq2.Parameters.Set("quality_grade",quality_grade);
			}

		    cmdinq2.ExecuteReader();
		    if(cmdinq2.Read())			/*cmdinq1*/
		    {
			    ht_slab_wt = cmdinq2.GetDecimal(1);
			    ht_slab_num = cmdinq2.GetDecimal(2);
		    }
		    cmdinq2.Close();
	    }
		bcls_ret->Tables[blkNum].Rows[0]["TT_SLAB_WT"] = tt_slab_wt;
		bcls_ret->Tables[blkNum].Rows[0]["TT_SLAB_NUM"] = tt_slab_num;
		bcls_ret->Tables[blkNum].Rows[0]["HT_SLAB_WT"] = ht_slab_wt;
		bcls_ret->Tables[blkNum].Rows[0]["HT_SLAB_NUM"] = ht_slab_num;
		
		
	
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
