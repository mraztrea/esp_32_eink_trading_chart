![1000034969](https://github.com/user-attachments/assets/f6fcbb30-dccb-4e75-885a-640d0c67eed7)

Ý tưởng ban đầu: hiển thị chart trên màn eink 3 màu, dùng màu trắng thể hiện nên tăng và đỏ thể hiện nên giảm. Update mỗi giá mỗi 15p, update xong thì sleep cho tiết kiệm pin, có thể dùng button để wake hoặc đổi qua chart tiếp theo.
Chart là vẽ từ a->z dựa trên dữ liệu giá không phải convert từ ảnh ra nên rất flexible. 
Đã làm xong tất cả cho đến khi vô phần backend thì mấy con API cấp giá chỉ cho phép lấy một số mã stock Mỹ, còn lại như forex phải trả phí nên chia tay ... Mình share code dưới comment, ae nếu có lực làm sv crawl data hoặc trả tiền api free thì đây là 1 cái chart di động khá đỉnh.
