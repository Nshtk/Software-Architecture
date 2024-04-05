DROP TABLE IF EXISTS public."User";
DROP TABLE IF EXISTS public."Accommodation";
DROP TABLE IF EXISTS public."Order";

CREATE TABLE IF NOT EXISTS public."User"
(
    id serial NOT NULL,
    login character varying COLLATE pg_catalog."default" NOT NULL,
    password character varying COLLATE pg_catalog."default" NOT NULL,
    first_name character varying COLLATE pg_catalog."default" NOT NULL,
    last_name character varying COLLATE pg_catalog."default" NOT NULL,
    email character varying COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT "User_pkey" PRIMARY KEY (id),
    CONSTRAINT "1" UNIQUE (login),
    CONSTRAINT "2" UNIQUE (email)
);
CREATE TABLE IF NOT EXISTS public."Accommodation"
(
    id serial NOT NULL,
    name character varying COLLATE pg_catalog."default" NOT NULL,
    description character varying COLLATE pg_catalog."default" NOT NULL,
    price integer,
    deadline date,
    CONSTRAINT "Accommodation_pkey" PRIMARY KEY (id)
);
CREATE TABLE IF NOT EXISTS public."Order"
(
    id serial NOT NULL,
    user_id integer NOT NULL,
    accommodation_id integer NOT NULL,
    CONSTRAINT "Order_pkey" PRIMARY KEY (id),
    CONSTRAINT fk_accommodation_id FOREIGN KEY (accommodation_id)
        REFERENCES public."Accommodation" (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
        NOT VALID,
    CONSTRAINT fk_user_id FOREIGN KEY (user_id)
        REFERENCES public."User" (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
        NOT VALID
);
--Only users iserted from code have hashed passwords
INSERT INTO public."User" (login, password, first_name, last_name, email) VALUES
('sample_login_1', '12345678', 'Sample First Name 1', 'Sample Last Name 1', 'sample_1@mail.ru'),
('sample_login_2', '123456789', 'Sample First Name 2', 'Sample Last Name 2', 'sample_2@mail.ru'),
('sample_login_3', '1234567891', 'Sample First Name 3', 'Sample Last Name 3', 'sample_3@mail.ru'),
('sample_login_4', '12345678912', 'Sample First Name 4', 'Sample Last Name 4', 'sample_4@mail.ru'),
('sample_login_5', '123456789123', 'Sample First Name 5', 'Sample Last Name 5', 'sample_5@mail.ru');

INSERT INTO public."Accommodation" (name, description, price, deadline) VALUES
('Sample accommodation 1', 'Sample accommodation description 1', 100, '2024-03-01'),
('Sample accommodation 2', 'Sample accommodation description 2', 200, '2024-03-02'),
('Sample accommodation 3', 'Sample accommodation description 3', 300, '2024-03-03');

INSERT INTO public."Order" (user_id, accommodation_id) VALUES
( 1, 1),
( 1, 2),
( 2, 1),
( 3, 2),
( 3, 3),
( 4, 1),
( 4, 2),
( 4, 3),
( 5, 3),

SELECT * FROM User;
SELECT * FROM Accommodation;
SELECT * FROM Order;