#Trả lời câu hỏi

1. Nếu bỏ qua tín hiệu SIGINT thì khi bấm Ctrl + C thì chương trình sẽ tiếp tục chạy mà không dừng nữa.
2. Nếu không gọi arlarm(1) thì hệ thống sẽ chỉ đếm lần đầu tiên và không tiếp tục gọi chương trình nữa.
3. Cha và con là hai process độc lập (dù cùng nguồn gốc). Cha không thể gọi trực tiếp hàm xử lý của con, vì mỗi process có không gian địa chỉ riêng.Chỉ có cách thông qua cơ chế IPC (Inter-Process Communication)s → nên phải dùng kill().
