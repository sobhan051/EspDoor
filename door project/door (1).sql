-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jan 01, 2024 at 05:50 AM
-- Server version: 8.0.35
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `door`
--

-- --------------------------------------------------------

--
-- Table structure for table `class`
--

CREATE TABLE `class` (
  `student_id` bigint NOT NULL,
  `first_name` varchar(50) COLLATE utf8mb3_persian_ci NOT NULL,
  `last_name` varchar(50) COLLATE utf8mb3_persian_ci NOT NULL,
  `uid` varchar(11) COLLATE utf8mb3_persian_ci NOT NULL,
  `class_day` int NOT NULL,
  `class_start_time` time NOT NULL,
  `class_end_time` time NOT NULL,
  `class_number` varchar(5) COLLATE utf8mb3_persian_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_persian_ci;

--
-- Dumping data for table `class`
--

INSERT INTO `class` (`student_id`, `first_name`, `last_name`, `uid`, `class_day`, `class_start_time`, `class_end_time`, `class_number`) VALUES
(4001277135, 'abas', 'nasrabadi', 'F8 41 8D 3F', 4, '13:00:00', '15:00:00', '251'),
(4011277133, 'amin', 'esmaeili', '2C 42 A3 50', 3, '09:00:00', '23:32:00', '251'),
(4011277140, 'mina', 'rezaei', 'C8 DB E6 3F', 3, '08:00:00', '13:30:00', '251'),
(4011277144, 'sobhan', 'askari', '5C 58 AB 51', 3, '09:00:00', '22:00:00', '251');

-- --------------------------------------------------------

--
-- Table structure for table `log`
--

CREATE TABLE `log` (
  `log_id` int NOT NULL,
  `student_id` bigint NOT NULL,
  `first_name` varchar(100) COLLATE utf8mb3_persian_ci NOT NULL,
  `last_name` varchar(100) COLLATE utf8mb3_persian_ci NOT NULL,
  `log_date` date NOT NULL,
  `log_time` time NOT NULL,
  `class_number` varchar(5) COLLATE utf8mb3_persian_ci NOT NULL,
  `authorised` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_persian_ci;

--
-- Dumping data for table `log`
--

INSERT INTO `log` (`log_id`, `student_id`, `first_name`, `last_name`, `log_date`, `log_time`, `class_number`, `authorised`) VALUES
(155, 0, 'NULL', 'NULL', '1402-10-11', '08:17:31', '251', 0),
(156, 0, 'NULL', 'NULL', '1402-10-11', '08:17:37', '251', 0),
(157, 0, 'NULL', 'NULL', '1402-10-11', '08:17:41', '251', 0),
(158, 0, 'NULL', 'NULL', '1402-10-11', '08:17:45', '251', 0),
(159, 4001277135, 'abas', 'nasrabadi', '1402-10-11', '08:17:48', '251', 0),
(160, 4011277140, 'mina', 'rezaei', '1402-10-11', '08:17:51', '251', 1),
(161, 4011277140, 'mina', 'rezaei', '1402-10-11', '08:19:23', '251', 1);

--
-- Indexes for dumped tables
--

--
-- Indexes for table `class`
--
ALTER TABLE `class`
  ADD PRIMARY KEY (`student_id`);

--
-- Indexes for table `log`
--
ALTER TABLE `log`
  ADD PRIMARY KEY (`log_id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `log`
--
ALTER TABLE `log`
  MODIFY `log_id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=162;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
