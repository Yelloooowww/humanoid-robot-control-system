from openpyxl.styles import PatternFill

fill = PatternFill(fill_type = None,start_color='FFFFFF',end_color='000000')

ws.['B1'].fill = fill


#保存文件
wb.save('test.xlsx')
