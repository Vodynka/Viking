contacts = {}

while True:
    print("1 - добавить, 2 - найти, 3 - все, 0 - выход")
    choice = input("> ").strip()

    if choice == "":
        continue

    if choice == "1":
        name = input("Имя: ").strip()
        if name == "":
            continue
        if name in contacts:
            print("Такой контакт уже есть!")
            continue
        phone = input("Телефон: ").strip()
        if phone == "":
            continue
        contacts[name] = phone
        print("Добавлено.")

    elif choice == "2":
        name = input("Имя: ").strip()
        if name in contacts:
            print("Телефон:", contacts[name])
        else:
            print("не найден")

    elif choice == "3":
        if not contacts:
            print("Справочник пуст.")
        for name, phone in contacts.items():
            print(name, "-", phone)

    elif choice == "0":
        print("Пока!")
        break

    else:
        print("Неверный пункт меню.")
