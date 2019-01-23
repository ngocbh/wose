# NOTES 

## Note 28/12/18
* Khi treat các từ trong từ điển ( = biến duringTrainingStage ) các từ không có trong từ điển dù ranking cao cũng ko bắt đc ( mặc dù quality > threshold ) bởi vì các từ đơn tạo thành từ đó bị treat nên khi cộng p các từ đơn lại lại lớn hơn từ mới bắt được ( không quan tâm đến kết quả ranking )
* Khi không treat, kết quả giảm rất nhiều. Miss rất nhiều từ trong từ điển. Nhưng đây là hướng cần cải thiện. Nên đi theo con đường này.
