workspace {
	name "Сайт заказа услуг"
	description "Сервис, с помощью которого пользователи могут заказывать и размещать услуги"

	!identifiers hierarchical
    !docs documentation
    !adrs decisions

    model {
		properties { 
            structurizr.groupSeparator "/"
        }

        user = person "Пользователь"
        accommodation = softwareSystem "Система поиска и размещения услуг"

		service = softwareSystem "Сервис заказа услуг" {
            description "Сервис"

            user_service = container "User service" {
                description "Сервис управления пользователями"
            }
			accommodation_service = container "accommodation service" {
                description "Сервис управления услугами"
            }
            order_service = container "Order service" {
                description "Сервис управления заказами"
            }

            group "Слой данных" {
                user_database = container "User Database" {
                    description "База данных для хранения информации о пользователях"
                    technology "PostgreSQL 15"
                    tags "database"
                }
                user_cache = container "User Cache" {
                    description "Кеш пользовательских данных для ускорения аутентификации"
                    technology "PostgreSQL 15"
                    tags "database"
                }
                accommodation_database = container "Accommodation Database" {
                    description "База данных для хранения информации об услугах и заказах"
                    technology "MongoDB 5"
                    tags "database"
                }
            }

            user_service -> user_cache "Получение/обновление данных о пользователях" "TCP 6379"
            user_service -> user_database "Получение/обновление данных о пользователях" "TCP 5432"
			accommodation_service -> accommodation_database "Получение/обновление данных об услугах" "TCP 4543"
			order_service -> accommodation_database "Получение/обновление данных о заказе" "TCP 6545"

            user -> user_service "Регистрация/аутентификация пользователя" "REST HTTP:8080"
            accommodation -> accommodation_service "Создание услуги" "REST HTTP:8081"

            order_service -> accommodation_service "Добавление услуг в заказ"
			user_service -> order_service "Получение заказа для пользователя"
        }

		deploymentEnvironment "Production" {
            deploymentNode "User Server" {
                containerInstance service.user_service
            }
            deploymentNode "Accomodation Server" {
                containerInstance service.accommodation_service
                properties {
                    "cpu" "4"
                    "ram" "256Gb"
                    "hdd" "4Tb"
                }
            }
            deploymentNode "Order Server" {
                containerInstance service.order_service
            }

            deploymentNode "databases" {
                deploymentNode "Database Server 1" {
                    containerInstance service.user_database
                }
                deploymentNode "Database Server 2" {
                    containerInstance service.accommodation_database
                    instances 3
                }
                deploymentNode "Cache Server" {
                    containerInstance service.user_cache
                }
            }
        }
    }

    views {
		theme default

		properties { 
            structurizr.tooltips true
        }

		!script groovy {
            workspace.views.createDefaultViews()
            workspace.views.views.findAll { it instanceof com.structurizr.view.ModelView }.each { it.enableAutomaticLayout() }
        }

		systemLandscape "Landscape" {
			include *
			autoLayout
		}

		/*systemContext service "Main"		//Views not working?
		{
			include service
			autoLayout
		}
		systemContext accommodation "Accomodation" 
		{
			include accommodation
			autoLayout
		}
		deployment service "Service"
		{
			include *
			autoLayout
		}
		container user_service "User_service"
		{
			include user_service
			autoLayout
		}*/

        dynamic service "UC01" "Добавление нового пользователя" {
            autoLayout
            user -> service.user_service "Создать нового пользователя (Swagger POST /user)"
            service.user_service -> service.user_database "Сохранить данные о пользователе" 
			service.user_service -> service.user_cache "Сохранить данные пользователя в кэш" 
        }
        dynamic service "UC02" "Поиск пользователя" {
            autoLayout
            user -> service.user_service "Найти пользователя (GET /user /string)"
			service.user_service -> service.user_cache "Получение данных пользователя из кэша"
            service.user_service -> service.user_database "ИЛИ Получение данных пользователя из базы данных"
        }
        dynamic service "UC03" "Создание услуги" {
            autoLayout
            accommodation -> service.accommodation_service "Создать новую услугу (POST /accomodation)"
            service.accommodation_service -> service.accommodation_database "Сохранить данные об услуге"
        }
        dynamic service "UC04" "Получение списка услуг" {
            autoLayout
            accommodation -> service.accommodation_service "Получение списка услуг (GET /accomodation)"
			service.accommodation_service -> service.accommodation_database "Получение информации из базы данных"
        }
        dynamic service "UC05" "Добавление услуг в заказ" {
            autoLayout
            user -> service.user_service "Создать заказ (POST /order)"
            service.user_service -> service.order_service "Добавить в заказ"
			service.order_service -> service.accommodation_service "Получить услугу"
			service.order_service -> service.accommodation_database "Обновить информацию о заказе"
        }
        dynamic service "UC06" "Получение заказа для пользователя" {
            autoLayout
            user -> service.user_service "Получить информациб о заказе (GET /order)"
            service.user_service -> service.order_service "Получить информацию о заказе для пользователя (GET /order)"
			service.order_service -> service.accommodation_database "Получить информацию о заказе из базы данных"
        }

        styles {
            element "database" {
                shape cylinder
            }
        }
    }
}
