#include "NBNumberBaseballLibrary.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNBInputValidationTest, "NB.Rules.InputValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNBInputValidationTest::RunTest(const FString& Parameters)
{
	const TArray<FString> InvalidInputs = {
		TEXT(""), TEXT("12"), TEXT("1234"), TEXT("112"), TEXT("121"), TEXT("122"),
		TEXT("012"), TEXT("abc"), TEXT("1a3"), TEXT(" 12"), TEXT("123 "),
		TEXT("１２３"), TEXT("-12"), TEXT("1.2"), TEXT("1\n3")
	};
	FString ErrorMessage;
	for (const FString& Input : InvalidInputs)
	{
		TestFalse(*FString::Printf(TEXT("Reject '%s'"), *Input),
			UNBNumberBaseballLibrary::ValidateInput(Input, ErrorMessage));
		TestFalse(TEXT("Invalid input has an explanation"), ErrorMessage.IsEmpty());
	}
	TestTrue(TEXT("Valid input accepted"), UNBNumberBaseballLibrary::ValidateInput(TEXT("386"), ErrorMessage));
	TestTrue(TEXT("Success clears previous error"), ErrorMessage.IsEmpty());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNBGuessEvaluationTest, "NB.Rules.GuessEvaluation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNBGuessEvaluationTest::RunTest(const FString& Parameters)
{
	struct FCase
	{
		const TCHAR* Input;
		int32 Strikes;
		int32 Balls;
		const TCHAR* Display;
	};
	const FCase Cases[] = {
		{ TEXT("127"), 0, 0, TEXT("OUT") },
		{ TEXT("167"), 0, 1, TEXT("0S1B") },
		{ TEXT("123"), 0, 1, TEXT("0S1B") },
		{ TEXT("368"), 1, 2, TEXT("1S2B") },
		{ TEXT("638"), 0, 3, TEXT("0S3B") },
		{ TEXT("381"), 2, 0, TEXT("2S0B") },
		{ TEXT("386"), 3, 0, TEXT("3S0B") }
	};
	FNBGuessResult Result;
	for (const FCase& Case : Cases)
	{
		TestTrue(Case.Input, UNBNumberBaseballLibrary::EvaluateGuess(TEXT("386"), Case.Input, Result));
		TestEqual(*FString::Printf(TEXT("%s strikes"), Case.Input), Result.Strikes, Case.Strikes);
		TestEqual(*FString::Printf(TEXT("%s balls"), Case.Input), Result.Balls, Case.Balls);
		TestEqual(*FString::Printf(TEXT("%s display"), Case.Input), Result.ToDisplayString(), FString(Case.Display));
	}
	TestFalse(TEXT("Invalid guess is not OUT"), UNBNumberBaseballLibrary::EvaluateGuess(TEXT("386"), TEXT("112"), Result));
	TestEqual(TEXT("Failure clears strikes"), Result.Strikes, 0);
	TestEqual(TEXT("Failure clears balls"), Result.Balls, 0);
	TestFalse(TEXT("Invalid answer rejected"), UNBNumberBaseballLibrary::EvaluateGuess(TEXT(""), TEXT("386"), Result));
	TestFalse(TEXT("Duplicate answer rejected"), UNBNumberBaseballLibrary::EvaluateGuess(TEXT("333"), TEXT("386"), Result));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNBValidNumbersTest, "NB.Rules.AllValidNumbers",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNBValidNumbersTest::RunTest(const FString& Parameters)
{
	int32 Count = 0;
	for (int32 Number = 111; Number <= 999; ++Number)
	{
		const int32 A = Number / 100;
		const int32 B = (Number / 10) % 10;
		const int32 C = Number % 10;
		if (B == 0 || C == 0 || A == B || B == C || A == C)
		{
			continue;
		}
		const FString Input = FString::FromInt(Number);
		FString ErrorMessage;
		FNBGuessResult Result;
		TestTrue(*Input, UNBNumberBaseballLibrary::ValidateInput(Input, ErrorMessage));
		TestTrue(TEXT("Self comparison succeeds"), UNBNumberBaseballLibrary::EvaluateGuess(Input, Input, Result));
		TestEqual(TEXT("Self comparison is 3S0B"), Result.ToDisplayString(), FString(TEXT("3S0B")));
		++Count;
	}
	TestEqual(TEXT("All 9 * 8 * 7 valid numbers covered"), Count, 504);
	return true;
}
#endif
