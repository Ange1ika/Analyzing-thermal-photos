import cv2
import os

# Папка с изображениями и папка для сохранения JPG
input_folder = 'Dataset/digital_images'
output_folder = 'Dataset/digital_images/output'

# Создаем папку для JPG, если её нет
os.makedirs(output_folder, exist_ok=True)

# Перебор всех файлов в папке
for filename in os.listdir(input_folder):
    input_path = os.path.join(input_folder, filename)
    
    # Загружаем изображение
    img = cv2.imread(input_path)
    
    if img is None:
        print(f"Не удалось открыть файл: {filename}")
        continue

    # Генерируем имя для JPG-файла
    output_path = os.path.join(output_folder, os.path.splitext(filename)[0] + '.jpg')

    # Сохраняем в формате JPG
    cv2.imwrite(output_path, img, [int(cv2.IMWRITE_JPEG_QUALITY), 90])
    print(f"Сохранено: {output_path}")

print("Конвертация завершена!")
